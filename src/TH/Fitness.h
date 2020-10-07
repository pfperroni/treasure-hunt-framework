/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file Fitness.h
 * @class Fitness
 * @author Peter Frank Perroni
 * @brief This structure represents the fitness (or cost) of one Solution.
 * @details A Fitness on TH can be represented by multiple values, instead of the traditional 1-value fitness
 *          (eg. multi-objective optimization, fitness history, score list, etc).
 *          The fitness is an ordered list with any number of elements, whose type must be
 *          of same basic data type for all elements (eg., double or int).
 */

#ifndef FITNESS_H_
#define FITNESS_H_

#include <stdexcept>

template <class F = double, int fSize = 1>
struct Fitness {
	F internalFitness[fSize];
	int size = fSize;

	void checkCompatibility(Fitness<F, fSize> *fitness) {
		if(fitness == NULL) {
			throw std::invalid_argument("Fitness can not be empty.");
		}
		if(size != fitness->size) {
			throw std::invalid_argument("Fitness's internal size is not compatible.");
		}
	}

public:
	/**
	 * @brief Operator that assigns the values of a buffer to the list
	 *        that represents the current Fitness.
	 *
	 * The buffer size must be compatible with this Fitness.
	 *
	 * @param buffer The source buffer to assign to the list.
	 */
	void operator =(F *buffer) {
		if (buffer == NULL) return;
		for(int i=0; i < size; i++){
			internalFitness[i] = buffer[i];
		}
	}

	/**
	 * @brief Operator that overrides the contents of this fitness with
	 *        the contents of the Fitness instance received.
	 * @param fitness The source Fitness instance.
	 * @throws invalid_argument if the source Fitness instance is not compatible with current fitness.
	 */
	void operator =(Fitness<F, fSize> *fitness){
		checkCompatibility(fitness);
		*this = fitness->internalFitness;
	}

	/**
	 * @brief Operator that overrides the contents of this fitness with
	 *        the contents of the Fitness instance received.
	 * @param fitness The source Fitness instance.
	 * @throws invalid_argument if the source Fitness instance is not compatible with current fitness.
	 */
	void operator =(Fitness<F, fSize> &fitness){
		checkCompatibility(&fitness);
		*this = fitness.internalFitness;
	}

	/**
	 * @brief Operator that assigns the same value to all elements of the list
	 *        that represents the current Fitness.
	 * @param value The value to assign to the list.
	 */
	void operator =(F value) {
		for(int i=0; i < size; i++){
			internalFitness[i] = value;
		}
	}

	/**
	 * @brief Compares the current fitness with the buffer received.
	 *
	 * The buffer size must be compatible with this Fitness.
	 *
	 * @param buffer The buffer to compare.
	 * @return True if this Fitness instance has the same contents as the
	 *         buffer received. False otherwise.
	 */
	bool equals(F *buffer){
		if(buffer == NULL) return false;
		for(int i=0; i < size; i++){
			if(this->internalFitness[i] != buffer[i]) return false;
		}
		return true;
	}

	/**
	 * @brief Compares the current fitness with the fitness received.
	 * @param fitness The Fitness instance to compare.
	 * @return True if this Fitness instance has the same contents as the
	 *         fitness received. False otherwise.
	 * @throws invalid_argument if the fitness received is not compatible with current fitness.
	 */
	bool equals(Fitness<F, fSize> *fitness){
		checkCompatibility(fitness);
		return equals(fitness->internalFitness);
	}
	bool operator ==(Fitness<F, fSize> *fitness){
		return fitness != NULL && equals(fitness);
	}
	bool operator ==(Fitness<F, fSize> &fitness){
		return fitness != NULL && equals(&fitness);
	}
	bool operator !=(Fitness<F, fSize> *fitness){
		return fitness == NULL || ! equals(fitness);
	}
	bool operator !=(Fitness<F, fSize> &fitness){
		return fitness == NULL || ! equals(&fitness);
	}

	/**
	 * @brief Get a partial fitness based on its index in the list of values that
	 *        represents the actual Fitness.
	 *
	 * Given that a Fitness can be represented by multiple values, a "partial fitness"
	 * here actually means one of the values that represents a single fitness (or cost).
	 *
	 * @param i The index of the partial fitness in the list (index list starts in zero).
	 * @return The partial fitness selected.
	 */
	F getInternalFitness(int i){
		if(i < 0 || i >= fSize) throw std::invalid_argument("Invalid index for internal fitness.");
		return internalFitness[i];
	}

	/**
	 * @brief Get a pointer to the actual list of values that represents the Fitness.
	 * @return The pointer to the fitness's values.
	 */
	F* getInternalFitness(){ return internalFitness; }

	/**
	 * @brief This method copies the contents of current Fitness to the
	 *        buffer received.
	 *
	 * The buffer size must be compatible with this Fitness.
	 *
	 * @param buffer The destination buffer.
	 */
	void getInternalFitness(F *buffer) {
		if(buffer == NULL) return;
		for(int i=0; i < size; i++){
			buffer[i] = internalFitness[i];
		}
	}

	/**
	 * @brief Get the first value from the list of values that represents the Fitness.
	 *
	 * Useful when the fitness is represented by 1 single value.
	 *
	 * @return The first value from the list of values that represents the fitness (or cost).
	 */
	F getFirstValue(){ return internalFitness[0]; }

	/**
	 * @brief Get the number of values that represents a Fitness.
	 * @return The number of values that represents a Fitness.
	 */
	int getFitnessSize() { return size; }
};

#endif /* FITNESS_H_ */

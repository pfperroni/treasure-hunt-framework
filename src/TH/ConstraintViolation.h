/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file ConstraintViolation.h
 * @class ConstraintViolation
 * @author Peter Frank Perroni
 * @brief This structure represents the constraints violated by one Solution.
 * @details The constraint violations are represented by an ordered list with
 *          any number of elements, whose type must be of same basic data type
 *          for all elements (eg., double or int).
 */

#ifndef CONSTRAINTVIOLATIONS_H_
#define CONSTRAINTVIOLATIONS_H_

#include <stdexcept>

template <class V = double, int vSize = 1>
struct ConstraintViolation {
	V internalViolations[vSize];
	int size = vSize;

	void checkCompatibility(ConstraintViolation<V, vSize> *violation) {
		if(violation == NULL) {
			throw std::invalid_argument("Violation can not be empty.");
		}
		if(size != violation->size) {
			throw std::invalid_argument("Violation's internal size is not compatible.");
		}
	}

public:
	/**
	 * @brief Operator that assigns the values of a buffer to the list
	 *        that represents the current ConstraintViolation.
	 * @param buffer The source buffer to assign to the list.
	 */
	void operator =(V *buffer) {
		if (buffer == NULL) return;
		for(int i=0; i < size; i++){
			internalViolations[i] = buffer[i];
		}
	}

	/**
	 * @brief Operator that overrides the contents of this list of
	 *        constraint violations with the contents of the
	 *        ConstraintViolation instance received.
	 * @param fitness The source ConstraintViolation instance.
	 * @throws invalid_argument if the source ConstraintViolation instance
	 *         is not compatible with current ConstraintViolation instance.
	 */
	void operator =(ConstraintViolation<V, vSize> *violation){
		checkCompatibility(violation);
		*this = violation->internalViolations;
	}

	/**
	 * @brief Operator that overrides the contents of this list of
	 *        constraint violations with the contents of the
	 *        ConstraintViolation instance received.
	 * @param fitness The source ConstraintViolation instance.
	 * @throws invalid_argument if the source ConstraintViolation instance
	 *         is not compatible with current ConstraintViolation instance.
	 */
	void operator =(ConstraintViolation<V, vSize> &violation){
		checkCompatibility(&violation);
		*this = violation.internalViolations;
	}

	/**
	 * @brief Operator that assigns the same value to all elements of the list
	 *        that represents the current ConstraintViolation.
	 * @param value The value to assign to the list.
	 */
	void operator =(V value) {
		for(int i=0; i < size; i++){
			internalViolations[i] = value;
		}
	}

	/**
	 * @brief Compares the current list of constraint violations with the buffer received.
	 * @param buffer The buffer to compare.
	 * @return True if this ConstraintViolation instance has the same contents as the
	 *         buffer received. False otherwise.
	 */
	bool equals(V *buffer){
		if(buffer == NULL) return false;
		for(int i=0; i < size; i++){
			if(this->internalViolations[i] != buffer[i]) return false;
		}
		return true;
	}

	/**
	 * @brief Compares the current list of constraint violations with the
	 *        ConstraintViolation instance received.
	 * @param violation The ConstraintViolation instance to compare.
	 * @return True if this ConstraintViolation instance has the same contents as the
	 *         one received. False otherwise.
	 * @throws invalid_argument if the ConstraintViolation instance received
	 *         is not compatible with current list of constraint violations.
	 */
	bool equals(ConstraintViolation<V, vSize> *violation){
		checkCompatibility(violation);
		return equals(violation->internalViolations);
	}
	bool operator ==(ConstraintViolation<V, vSize> *violation){
		return violation != NULL && equals(violation);
	}
	bool operator ==(ConstraintViolation<V, vSize> &violation){
		return violation != NULL && equals(&violation);
	}
	bool operator !=(ConstraintViolation<V, vSize> *violation){
		return violation == NULL || ! equals(violation);
	}
	bool operator !=(ConstraintViolation<V, vSize> &violation){
		return violation == NULL || ! equals(&violation);
	}

	/**
	 * @brief Get a constraint violation based on its index in the list of violations.
	 * @param i The index of the constraint violation in the list (index list starts in zero).
	 * @return The constraint violation selected.
	 */
	V getInternalViolation(int i){
		if(i < 0 || i >= vSize) throw std::invalid_argument("Invalid index for internal violation.");
		return internalViolations[i];
	}

	/**
	 * @brief Get a pointer to the actual list of values that represents the ConstraintViolation.
	 * @return The pointer to the constraint violation's values.
	 */
	V* getInternalViolation(){ return internalViolations; }

	/**
	 * @brief This method copies the contents of current ConstraintViolation to the
	 *        buffer received.
	 * @param buffer The destination buffer.
	 */
	void getInternalViolation(V *buffer) {
		if(buffer == NULL) return;
		for(int i=0; i < size; i++){
			buffer[i] = internalViolations[i];
		}
	}

	/**
	 * @brief Get the first value from the list of values that
	 *        represents the ConstraintViolation.
	 *
	 * Useful when the problem has only a single constraint.
	 *
	 * @return The first value from the list of values that represents the constraint violations.
	 */
	V getFirstValue(){ return internalViolations[0]; }

	/**
	 * @brief Get the number of values that represents a ConstraintViolation.
	 * @return The number of values that represents a ConstraintViolation.
	 */
	int getViolationSize() { return size; }
};

#endif /* CONSTRAINTVIOLATIONS_H_ */

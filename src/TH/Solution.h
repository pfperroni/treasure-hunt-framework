/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For additional notifications, please check the file NOTICE.txt.
 *
 *
 * @file Solution.h
 * @class Solution
 * @author Peter Frank Perroni
 * @brief This class represents a population individual.
 * @details In TH, a Solution is comprised of a candidate solution (represented by a list of Positions),
 *          its fitness and all constraints it has violated (if any).
 */

#ifndef SOLUTION_H_
#define SOLUTION_H_

#include "ConstraintViolation.h"
#include "Position.h"
#include "Fitness.h"
#include "Region.h"
#include "THUtil.h"

#include <mpi.h>
#include <stddef.h>
#include <stdexcept>
#include <string>

template<class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class Solution {
	Position<P, pSize> *positions;
	Fitness<F, fSize> fitness;
	ConstraintViolation<V, vSize> violation;
	int n;
	unsigned int seed;

	void setup(int nDimensions) {
		if (nDimensions <= 0) throw std::invalid_argument("The number of dimensions must be greater than zero.");
		n = nDimensions;
		positions = new Position<P, pSize>[n];
		seed = THUtil::getRandomSeed();
	}

	void checkCompatibility(Solution<P, pSize, F, fSize, V, vSize> *solution) {
		if(solution == NULL) {
			throw std::invalid_argument("Solution can not be empty.");
		}
		if(n != solution->n) {
			throw std::invalid_argument(std::string("Solution's internal sizes are not compatible [")
										+ std::to_string(n) + " != " + std::to_string(solution->n) + "].");
		}
	}

public:
	/**
	 * @brief This constructor creates an empty Solution instance.
	 * @param nDimensions The number of dimensions this solution will contain.
	 */
	Solution(int nDimensions) {
		setup(nDimensions);
	}

	/**
	 * @brief This constructor creates a Solution instance by copying
	 *        all contents of another Solution instance.
	 * @param solution The source Solution instance.
	 */
	Solution(Solution<P, pSize, F, fSize, V, vSize> *solution) { // @suppress("Class members should be properly initialized")
		if (solution == NULL) throw std::invalid_argument("The original solution is empty.");
		setup(solution->getNDimensions());
		*this = solution;
	}
	~Solution() {
		delete positions;
	}

	/**
	 * @brief This operator overrides the solution's Position with the contents of the
	 *        buffer received.
	 *
	 * The buffer size must be compatible with this solution's Position.
	 *
	 * @param buffer The source buffer.
	 */
	void operator =(P *buffer) {
		if (buffer == NULL) return;
		for (int i=0, ptr=0; i < n; i++, ptr+=pSize) {
			positions[i] = &buffer[ptr];
		}
	}

	/**
	 * @brief Operator that overrides the solution's Position, Fitness and Violation
	 *        with the contents of the Solution received.
	 *
	 * @param solution The source Solution instance.
	 * @throws invalid_argument if the source Solution instance is not compatible with current solution.
	 */
	void operator =(Solution<P, pSize, F, fSize, V, vSize> *solution) {
		checkCompatibility(solution);
		this->n = solution->n;
		for (int i = 0; i < solution->n; i++) {
			this->positions[i] = solution->positions[i];
		}
		this->fitness = solution->fitness;
		this->violation = solution->violation;
	}

	void operator =(Solution<P, pSize, F, fSize, V, vSize> &solution) {
		*this = &solution;
	}

	/**
	 * @brief Compares the current solution with the solution received.
	 * @param solution The Solution instance to compare.
	 * @return True if this Solution instance has the same contents as the
	 *         solution received. False otherwise.
	 * @throws invalid_argument if the Solution instance received is not compatible with current solution.
	 */
	bool equals(Solution<P, pSize, F, fSize, V, vSize> *solution) {
		checkCompatibility(solution);
		if (this->n != solution->n) return false;
		for (int i = 0; i < solution->n; i++) {
			if (this->positions[i] != solution->positions[i]) return false;
		}
		if (this->fitness != solution->fitness) return false;
		if (this->violation != solution->violation) return false;
		return true;
	}
	bool operator ==(Solution<P, pSize, F, fSize, V, vSize> *solution) {
		return solution != NULL && equals(solution);
	}
	bool operator ==(Solution<P, pSize, F, fSize, V, vSize> &solution) {
		return solution != NULL && equals(&solution);
	}
	bool operator !=(Solution<P, pSize, F, fSize, V, vSize> *solution) {
		return solution == NULL || ! equals(solution);
	}
	bool operator !=(Solution<P, pSize, F, fSize, V, vSize> &solution) {
		return solution == NULL || ! equals(&solution);
	}

	/**
	 * @brief Operator that selects a Position based on its index in the list of positions.
	 *
	 * The pointer to the actual object is returned, instead of a simple copy.
	 *
	 * @param i The index of the Position in the list (index list starts in zero).
	 * @return A pointer to the Position selected.
	 */
	Position<P, pSize>* operator [](int i) {
		return getPosition(i);
	}

	/**
	 * @brief Get a Position based on its index in the list of positions.
	 *
	 * The pointer to the actual object is returned, instead of a simple copy.
	 *
	 * @param i The index of the Position in the list (index list starts in zero).
	 * @return The Position selected.
	 */
	Position<P, pSize>* getPosition(int i) {
		if (i < 0 || i >= n) {
			throw std::invalid_argument(std::string("Invalid index for solution [")
													+ std::to_string(i) + "].");
		}
		return &positions[i];
	}

	/**
	 * @brief This method copies the contents of current Position to the
	 *        buffer received.
	 *
	 * The buffer size must be compatible with this solution's Position.
	 *
	 * @param buffer The destination buffer.
	 */
	void getPositions(P *buffer) {
		if (buffer == NULL) return;
		for (int i = 0, ptr = 0; i < n; i++, ptr+=pSize) {
			positions[i].getInternalPosition(&buffer[ptr]);
		}
	}

	/**
	 * @brief Get a pointer to the actual Fitness instance.
	 * @return The pointer to the Fitness instance.
	 */
	Fitness<F, fSize>* getFitness() {
		return &fitness;
	}

	/**
	 * @brief This method copies the contents of current Fitness to the
	 *        buffer received.
	 *
	 * The buffer size must be compatible with this solution's Fitness.
	 *
	 * @param buffer The destination buffer.
	 */
	void getFitness(F *buffer) {
		if (buffer == NULL) return;
		fitness.getInternalFitness(buffer);
	}

	/**
	 * @brief Assigns the values of the buffer received to the
	 *        current Fitness instance.
	 *
	 * The buffer size must be compatible with this solution's Fitness.
	 *
	 * @param buffer The source buffer.
	 */
	void setFitness(F *buffer) {
		this->fitness = buffer;
	}

	/**
	 * @brief Assigns the same value to all elements of the Fitness instance.
	 * @param value The value to assign to the Fitness instance.
	 */
	void setFitness(F value) {
		this->fitness = value;
	}

	/**
	 * @brief Get a pointer to the actual ConstraintViolation instance.
	 * @return The pointer to the ConstraintViolation instance.
	 */
	ConstraintViolation<V, vSize>* getViolation() {
		return &violation;
	}

	/**
	 * @brief This method copies the contents of current ConstraintViolation to the
	 *        buffer received.
	 *
	 * The buffer size must be compatible with this solution's ConstraintViolation instance.
	 *
	 * @param buffer The destination buffer.
	 */
	void getViolation(V *buffer) {
		if (buffer == NULL) return;
		fitness.getInternalViolation(buffer);
	}

	/**
	 * @brief Assigns the values of the buffer received to the
	 *        current ConstraintViolation instance.
	 *
	 * The buffer size must be compatible with this solution's ConstraintViolation instance.
	 *
	 * @param buffer The source buffer.
	 */
	void setViolation(V *buffer) {
		this->violation = buffer;
	}

	/**
	 * @brief Assigns the same value to all elements of the ConstraintViolation instance.
	 * @param value The value to assign to the ConstraintViolation instance.
	 */
	void setViolation(F value) {
		this->violation = value;
	}

	/**
	 * @brief Reset the position of the current Solution in the search space.
	 *
	 * If a bias is provided, every the Position instance is reset as follows (with 50% of chance):
	 * - Close to the bias;
	 * - At same Position as the bias.
	 *
	 * If the new Position violates the boundaries constraints, it will be set to the boundary Position.
	 *
	 * @param r The "anchor" Region for the current Solution.
	 * @param bias The bias to guide the reset.
	 */
	void reset(Region<P> *r, Solution<P, pSize, F, fSize, V, vSize> *bias = NULL) {
		if (r == NULL) throw std::invalid_argument("Region cannot be null.");
		map<int, Dimension<P>*> *originalDimensions = r->getOriginalDimensions();
		if (n != (int) originalDimensions->size()) {
			throw std::invalid_argument("The number of dimensions does not match.");
		}

		map<Dimension<P>*, Partition<P>*> *partitions = r->getPartitions();
		Partition<P> *partition;
		Position<P, pSize> *pos;
		for (int i = 0; i < n; i++) {
			// Use originalDimensions to map the correct sequence of dimensions on partitions.
			partition = (*partitions)[(*originalDimensions)[i]];
			if(bias != NULL) { // If a bias is provided.
				pos = (*bias)[i];
				if(THUtil::randUniformDouble(seed, 0, 1) < 0.5) { // For half of the dimensions:
					// Reset close to the bias.
					positions[i] = THUtil::randNormalDouble(seed, pos->getLowerValue()*0.99, pos->getUpperValue()*1.01);
				} // For the other half:
				else positions[i] = pos; // Reset exactly on the same position as the bias.
			}
			else { // If no bias is provides:
				// Reset to a random position within original "anchor" sub-region.
				positions[i] = THUtil::randUniformDouble(seed, partition->getStartPoint(), partition->getEndPoint());
			}
			// Make sure the new position is inside the boundaries.
			positions[i].adjustUpperBound(partition->getEndPoint());
			positions[i].adjustLowerBound(partition->getStartPoint());
		}
	}

	/**
	 * @brief Get the number of dimensions of this Solution instance.
	 * @return the number of dimensions of this solution.
	 */
	int getNDimensions() {
		return n;
	}
};

#endif /* SOLUTION_H_ */

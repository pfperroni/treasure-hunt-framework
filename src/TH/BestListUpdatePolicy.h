/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file BestListUpdatePolicy.h
 * @class BestListUpdatePolicy
 * @author Peter Frank Perroni
 * @brief Template for the policy that specifies how the solutions
 *        within the BestList are update.
 */

#ifndef BESTLISTUPDATEPOLICY_H_
#define BESTLISTUPDATEPOLICY_H_

#include "BestList.h"
#include "FitnessPolicy.h"
#include <cmath>

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class BestListUpdatePolicy {
protected:
	/**
	 * @brief Convenience method that calculates the Euclidean distance between two solutions.
	 * @param first The first solution.
	 * @param second The second solution.
	 * @return A double number representing the Euclidean distance between the solutions.
	 */
	double euclideanDistance(Solution<P, pSize, F, fSize, V, vSize> *first, Solution<P, pSize, F, fSize, V, vSize> *second){
		if(first == NULL || second == NULL){
			throw std::invalid_argument("The solutions cannot be NULL.");
		}
		int n = first->getNDimensions();
		if(n != second->getNDimensions()){
			throw std::invalid_argument("The sizes of the solutions do not match.");
		}
		double dist = 0;
		Position<P, pSize> tmpPos;
		for(int i=0; i < n; i++) {
			tmpPos = (*second)[i];
			tmpPos.sub((*first)[i]);
			dist += pow(tmpPos.sumInternalValues(), 2);
		}
		return dist;
	}

public:
	BestListUpdatePolicy() {}
	virtual ~BestListUpdatePolicy() {}

	/**
	 * @brief Virtual method to apply the policy that will update the best-list.
	 *
	 * A method implementing this virtual method is the responsible for actually
	 * updating the best-list.
	 * It should use the FitnessPolicy received to compare the new solution with
	 * the solutions stored in the best-list.
	 *
	 * @param bestList The BestList instance to be updated.
	 * @param solution The new solution to be added to the best-list.
	 * @param fitnessPolicy The FitnessPolicy instance capable of evaluating the solutions.
	 */
	virtual void apply(BestList<P, pSize, F, fSize, V, vSize> *bestList,
			Solution<P, pSize, F, fSize, V, vSize> *solution,
			FitnessPolicy<P, pSize, F, fSize, V, vSize> *fitnessPolicy) = 0;
};

#endif /* BESTLISTUPDATEPOLICY_H_ */

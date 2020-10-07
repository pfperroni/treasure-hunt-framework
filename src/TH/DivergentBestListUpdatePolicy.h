/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file DivergentBestListUpdatePolicy.h
 * @class DivergentBestListUpdatePolicy
 * @author Peter Frank Perroni
 * @brief This policy updates the BestList instance by enforcing a behavior that
 *        slows down the convergence, as an attempt to increase the exploration
 *        and, consequently, the diversity of solutions.
 */

#ifndef DIVERGENTBESTLISTUPDATEPOLICY_H_
#define DIVERGENTBESTLISTUPDATEPOLICY_H_

#include "BestListUpdatePolicy.h"

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class DivergentBestListUpdatePolicy : public BestListUpdatePolicy<P, pSize, F, fSize, V, vSize> {
public:
	DivergentBestListUpdatePolicy() {}
	~DivergentBestListUpdatePolicy(){}

	/**
	 * @brief This method implements a behavior that promotes diversity when updating
	 * the solutions from the BestList instance.
	 *
	 * @param bestList The BestList instance to be updated.
	 * @param solution The new solution to be added to the best-list.
	 * @param fitnessPolicy The FitnessPolicy instance capable of evaluating the solutions.
	 */
	void apply(BestList<P, pSize, F, fSize, V, vSize> *bestList,
			Solution<P, pSize, F, fSize, V, vSize> *solution,
			FitnessPolicy<P, pSize, F, fSize, V, vSize> *fitnessPolicy) {
		if(bestList == NULL || solution == NULL || fitnessPolicy == NULL) {
			throw std::invalid_argument("All parameters must be provided to update the best list.");
		}

		int worst = -1;
		double smallestDistance = DBL_MAX, distance;
		for(int i=0; i < bestList->getListSize(); i++){
			// Occupy the empty positions first.
			if((*bestList)[i] == NULL) {
				worst = i;
				break;
			}
			else if(fitnessPolicy->firstIsBetter(solution, (*bestList)[i])){
				distance = this->euclideanDistance(solution, (*bestList)[i]);
				// Maximize the diversity replacing the smallest Cartesian distance.
				if(distance < smallestDistance){
					smallestDistance = distance;
					worst = i;
				}
			}
		}
		if(worst > -1){
			Solution<P, pSize, F, fSize, V, vSize> *worstSolution = (*bestList)[worst];
			if(worstSolution == NULL) {
				bestList->set(worst, new Solution<P, pSize, F, fSize, V, vSize>(solution));
			}
			else *worstSolution = solution;
		}
	}
};

#endif /* DIVERGENTBESTLISTUPDATEPOLICY_H_ */

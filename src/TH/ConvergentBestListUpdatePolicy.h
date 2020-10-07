/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file ConvergentBestListUpdatePolicy.h
 * @class ConvergentBestListUpdatePolicy
 * @author Peter Frank Perroni
 * @brief This policy updates the BestList instance by enforcing a behavior that
 *        speeds up the convergence, as an attempt to reduce the time required to
 *        converge and, consequently, increase communication between TH instances.
 */

#ifndef CONVERGENTBESTLISTUPDATEPOLICY_H_
#define CONVERGENTBESTLISTUPDATEPOLICY_H_

#include "BestListUpdatePolicy.h"

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class ConvergentBestListUpdatePolicy : public BestListUpdatePolicy<P, pSize, F, fSize, V, vSize> {
public:
	ConvergentBestListUpdatePolicy() {}
	~ConvergentBestListUpdatePolicy(){}

	/**
	 * @brief This method implements a behavior that focus on convergence speed when updating
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
		double largestDistance = -1, distance;
		for(int i=0; i < bestList->getListSize(); i++){
			// Occupy the empty positions first.
			if((*bestList)[i] == NULL) {
				worst = i;
				break;
			}
			else if(fitnessPolicy->firstIsBetter(solution, (*bestList)[i])){
				distance = this->euclideanDistance(solution, (*bestList)[i]);
				// Minimize the diversity replacing the largest Cartesian distance.
				if(distance > largestDistance){
					largestDistance = distance;
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

#endif /* CONVERGENTBESTLISTUPDATEPOLICY_H_ */

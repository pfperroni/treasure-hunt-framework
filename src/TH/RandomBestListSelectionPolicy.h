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
 * @file RandomBestListSelectionPolicy.h
 * @class RandomBestListSelectionPolicy
 * @brief This policy selects a random Solution from the BestList instance.
 */

#ifndef RANDOMBESTLISTSELECTIONPOLICY_H_
#define RANDOMBESTLISTSELECTIONPOLICY_H_

#include "BestListSelectionPolicy.h"

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class RandomBestListSelectionPolicy : public BestListSelectionPolicy<P, pSize, F, fSize, V, vSize> {
	unsigned int seed;

public:
	RandomBestListSelectionPolicy() {
		seed = THUtil::getRandomSeed();
	}
	~RandomBestListSelectionPolicy(){}

	/**
	 * @brief Implements the policy that selects a random solution from the best-list.
	 *
	 * @param bestList The BestList instance.
	 * @param fitnessPolicy The FitnessPolicy instance capable of evaluating the solutions.
	 * @return A random solution selected by this policy.
	 */
	Solution<P, pSize, F, fSize, V, vSize>* apply(BestList<P, pSize, F, fSize, V, vSize> *bestList,
			FitnessPolicy<P, pSize, F, fSize, V, vSize> *fitnessPolicy){
		if(bestList == NULL || bestList->getListSize() == 0) {
			throw std::invalid_argument("The best list cannot be empty.");
		}
		int bestListSz = bestList->getListSize();
		int pos = THUtil::randUniformInt(seed, 0, bestListSz-1); // Try a random solution from best-list.
		Solution<P, pSize, F, fSize, V, vSize> *selectedSolution = (*bestList)[pos];
		if(selectedSolution == NULL){ // If not filled, get the first solution available.
			for(pos=0; pos < bestListSz && (selectedSolution=(*bestList)[pos]) == NULL; pos++);
		}
		return selectedSolution;
	}
};

#endif /* RANDOMBESTLISTSELECTIONPOLICY_H_ */

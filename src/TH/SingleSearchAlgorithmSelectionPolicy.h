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
 * @file SingleSearchAlgorithmSelectionPolicy.h
 * @class SingleSearchAlgorithmSelectionPolicy
 * @author Peter Frank Perroni
 * @brief This class executes always the first Search instance available.
 */

#ifndef SINGLESEARCHALGORITHMSELECTIONPOLICY_H_
#define SINGLESEARCHALGORITHMSELECTIONPOLICY_H_

#include "SearchAlgorithmSelectionPolicy.h"

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class SingleSearchAlgorithmSelectionPolicy : public SearchAlgorithmSelectionPolicy<P, pSize, F, fSize, V, vSize> {

public:
	SingleSearchAlgorithmSelectionPolicy() {}
	~SingleSearchAlgorithmSelectionPolicy() {}

	void rank(
			int ID, THTree *tree, vector<SearchScore<P, pSize, F, fSize, V, vSize>*>* algorithms,
			Search<P, pSize, F, fSize, V, vSize>* currAlg, Fitness<F, fSize> *currFitness,
			int currentNEvals, long long nEvals) {
		if(algorithms == NULL || algorithms->size() == 0) {
			throw std::invalid_argument("The list of algorithms is empty.");
		}
		(*algorithms)[0]->setScore(1);
	}

	Search<P, pSize, F, fSize, V, vSize>* apply(
			int ID, THTree *tree, vector<SearchScore<P, pSize, F, fSize, V, vSize>*>* algorithms) {
		if(algorithms == NULL || algorithms->size() == 0) {
			throw std::invalid_argument("The list of algorithms is empty.");
		}
		return (*algorithms)[0]->getSearchAlgorithm();
	}
};

#endif /* SINGLESEARCHALGORITHMSELECTIONPOLICY_H_ */

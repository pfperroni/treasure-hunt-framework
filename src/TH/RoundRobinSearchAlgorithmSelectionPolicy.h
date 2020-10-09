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
 * @file RoundRobinSearchAlgorithmSelectionPolicy.h
 * @class RoundRobinSearchAlgorithmSelectionPolicy
 * @author Peter Frank Perroni
 * @brief This class executes Search instances in round-robin.
 */

#ifndef ROUNDROBINSEARCHALGORITHMSELECTIONPOLICY_H_
#define ROUNDROBINSEARCHALGORITHMSELECTIONPOLICY_H_

#include "SearchAlgorithmSelectionPolicy.h"

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class RoundRobinSearchAlgorithmSelectionPolicy : public SearchAlgorithmSelectionPolicy<P, pSize, F, fSize, V, vSize> {

	unsigned int currAlgPos;

public:
	RoundRobinSearchAlgorithmSelectionPolicy() {
		currAlgPos = -1;
	}
	~RoundRobinSearchAlgorithmSelectionPolicy() {}

	void rank(
			int ID, THTree *tree, vector<SearchScore<P, pSize, F, fSize, V, vSize>*>* algorithms,
			Search<P, pSize, F, fSize, V, vSize>* currAlg, Fitness<F, fSize> *currFitness,
			int currentNEvals, long long nEvals) {
		if(algorithms == NULL || algorithms->size() == 0) {
			throw std::invalid_argument("The list of algorithms is empty.");
		}
		(*algorithms)[currAlgPos]->setScore(1);
	}

	Search<P, pSize, F, fSize, V, vSize>* apply(
			int ID, THTree *tree, vector<SearchScore<P, pSize, F, fSize, V, vSize>*>* algorithms) {
		if(algorithms == NULL || algorithms->size() == 0) {
			throw std::invalid_argument("The list of algorithms is empty.");
		}
		if(++currAlgPos == algorithms->size()) currAlgPos = 0;
		return (*algorithms)[currAlgPos]->getSearchAlgorithm();
	}
};

#endif /* ROUNDROBINSEARCHALGORITHMSELECTIONPOLICY_H_ */

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
 * @file BestListSelectionPolicy.h
 * @class BestListSelectionPolicy
 * @author Peter Frank Perroni
 * @brief Template for the policy that specifies how a solution
 *        is selected from the BestList.
 */

#ifndef BESTLISTSELECTIONPOLICY_H_
#define BESTLISTSELECTIONPOLICY_H_

#include "BestList.h"

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class BestListSelectionPolicy {
public:
	BestListSelectionPolicy() {}
	virtual ~BestListSelectionPolicy() {}

	/**
	 * @brief Virtual method to apply the policy that will select a solution from the best-list.
	 *
	 * A method implementing this virtual method is the responsible for selecting
	 * and returning a solution from the best-list.
	 * It should use the FitnessPolicy received to compare the new solution with
	 * the solutions stored in the best-list.
	 *
	 * @param bestList The BestList instance.
	 * @param fitnessPolicy The FitnessPolicy instance capable of evaluating the solutions.
	 * @return The solution selected by the policy.
	 */
	virtual Solution<P, pSize, F, fSize, V, vSize>* apply(BestList<P, pSize, F, fSize, V, vSize> *bestList,
			FitnessPolicy<P, pSize, F, fSize, V, vSize> *fitnessPolicy) = 0;
};

#endif /* BESTLISTSELECTIONPOLICY_H_ */

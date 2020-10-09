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
 * @file RegionSelectionPolicy.h
 * @class RegionSelectionPolicy
 * @author Peter Frank Perroni
 * @brief Template for the policy that implements the sub-region selection criteria.
 * @details The class that implements this template must partition the search space
 *        matching the tree topology, and then choose one "anchor" sub-region according
 *        to the TH instance's ID.
 */

#ifndef REGIONSELECTIONPOLICY_H_
#define REGIONSELECTIONPOLICY_H_

#include "THTree.h"
#include "SearchSpace.h"
#include <map>

using namespace std;

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class RegionSelectionPolicy {
public:
	RegionSelectionPolicy() {}
	virtual ~RegionSelectionPolicy() {}

	/**
	 * @brief Virtual method to apply the policy that will choose one "anchor" sub-region
	 * according to the TH instance's ID.
	 *
	 * A method implementing this virtual method is the responsible for partitioning the
	 * search space matching the tree topology, so that the "anchor" sub-region can be
	 * chosen.
	 * The caller is responsible for deleting (freeing up the memory of) the Region instance returned.
	 *
	 * @param S The search space to be partitioned.
	 * @param tree The tree topology that will guide the partitioning.
	 * @param ID The TH instance's global ID.
	 * @return The "anchor" sub-region for current TH instance.
	 */
	virtual Region<P>* apply(SearchSpace<P> *S, THTree *tree, int ID) = 0;

	/**
	 * @brief This method can be used by sub-classes to obtain a dynamic region
	 *        at every TH iteration.
	 *
	 * By default, it returns the current "anchor" sub-region (no dynamic behavior!).
	 *
	 * EXPERIMENTAL: Use it with caution !!!
	 *
	 * @param iterationData The IterationData instance.
	 * @param S The full search space.
	 * @param R The current "anchor" sub-region.
	 * @param tree The tree topology that will guide the partitioning.
	 * @param ID The TH instance's global ID.
	 * @return The new "anchor" sub-region generated dynamically for current TH instance.
	 */
	Region<P>* recalculate(IterationData<P, pSize, F, fSize, V, vSize> *iterationData,
			SearchSpace<P> *S, Region<P> *R, THTree *tree, int ID){
		return R;
	}
};

#endif /* REGIONSELECTIONPOLICY_H_ */

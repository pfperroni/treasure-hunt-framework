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
 * @file RelocationStrategyData.h
 * @class RelocationStrategyData
 * @author Peter Frank Perroni
 * @brief Repository class that stores useful data required for
 *        the population's relocation strategy.
 */

#ifndef RELOCATIONSTRATEGYDATA_H_
#define RELOCATIONSTRATEGYDATA_H_

#include "IterationData.h"

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
struct RelocationStrategyData {
	IterationData<P, pSize, F, fSize, V, vSize> *iterationData;

public:
	RelocationStrategyData() {
		iterationData = NULL;
	}
	~RelocationStrategyData() {}

	/**
	 * @brief Set the IterationData instance that stores useful data about the TH iterations.
	 * @param iterationData The IterationData instance.
	 */
	void setIterationData(IterationData<P, pSize, F, fSize, V, vSize> *iterationData) {
		this->iterationData = iterationData;
	}

	IterationData<P, pSize, F, fSize, V, vSize>* getIterationData() {
		return iterationData;
	}
};

#endif /* RELOCATIONSTRATEGYDATA_H_ */

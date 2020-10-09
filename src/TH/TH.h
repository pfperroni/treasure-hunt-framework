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
 * @file TH.h
 * @class TH
 * @author Peter Frank Perroni
 * @brief This class interfaces the communication between
 *        the TH mechanisms and the external caller.
 */

#ifndef TH_H_
#define TH_H_

#include "Solution.h"
#include "BestList.h"
#include <mpi.h>

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class TH {
public:
	TH() {}
	virtual ~TH() {}

	/**
	 * @brief Starts the Treasure Hunt mechanisms.
	 */
	virtual void run() = 0;

	/**
	 * @brief Get the best Solution obtained after the TH has finished.
	 * @return The final best Solution.
	 */
	virtual Solution<P, pSize, F, fSize, V, vSize>* getBestSolution() = 0;

	/**
	 * @brief Get the best-list obtained after the TH has finished.
	 * @return The final BestList instance.
	 */
	virtual BestList<P, pSize, F, fSize, V, vSize>* getBestList() = 0;

	/**
	 * @brief Get the number of evaluations performed after the TH has finished.
	 * @return The total number of evaluations performed.
	 */
	virtual long long getNEvals() = 0;

	/**
	 * @brief Get this TH instance's unique identifier.
	 * @return This TH instance's unique identifier.
	 */
	virtual int getID() = 0;
};

#endif /* TH_H_ */

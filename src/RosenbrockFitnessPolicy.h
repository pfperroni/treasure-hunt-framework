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
 * @file RosenbrockFitnessPolicy.h
 * @class RosenbrockFitnessPolicy
 * @author Peter Frank Perroni
 * @brief Implementation of the Rosenbrock function for TH.
 */

#ifndef ROSENBROCKFITNESSPOLICY_H_
#define ROSENBROCKFITNESSPOLICY_H_

#include "TH/FitnessPolicy.h"
#include "TH/Solution.h"
#include <mpi.h>
#include <cfloat>

class RosenbrockFitnessPolicy : public FitnessPolicy<> {
public:
	RosenbrockFitnessPolicy(){}
	~RosenbrockFitnessPolicy(){}

	void apply(Solution<>* solution);

	bool firstIsBetter(Solution<>* first, Solution<>* second);

	bool firstIsBetter(Fitness<> *first, Fitness<> *second);

	void setWorstFitness(Solution<>* solution);

	void setWorstFitness(Fitness<> *fitness);

	void setBestFitness(Solution<>* solution);

	void setBestFitness(Fitness<> *fitness);

	double getMinEstimatedFitnessValue() { return 0; }
};

#endif /* ROSENBROCKFITNESSPOLICY_H_ */

/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
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

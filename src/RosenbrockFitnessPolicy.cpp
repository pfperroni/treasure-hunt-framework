/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file RosenbrockFitnessPolicy.cpp
 * @class RosenbrockFitnessPolicy
 * @author Peter Frank Perroni
 * @brief Implementation of the Rosenbrock function for TH.
 */

#include "RosenbrockFitnessPolicy.h"

void RosenbrockFitnessPolicy::apply(Solution<>* solution) {
	double fitness = 0, x1, x2;
	for(int i=0; i < solution->getNDimensions()-1; i++){
		x1 = (*solution)[i]->getFirstValue();
		x2 = (*solution)[i+1]->getFirstValue();
		fitness += (1-x1)*(1-x1) + 100 * (x2-x1*x1) * (x2-x1*x1);
	}
	solution->setFitness(fitness);
}

bool RosenbrockFitnessPolicy::firstIsBetter(Solution<>* first, Solution<>* second) {
	if(first != NULL && second == NULL) return true;
	else if(first == NULL && second != NULL) return false;
	else if(first == NULL && second == NULL) return false;
	return first->getFitness()->getFirstValue() < second->getFitness()->getFirstValue();
}

bool RosenbrockFitnessPolicy::firstIsBetter(Fitness<> *first, Fitness<> *second) {
	if(first != NULL && second == NULL) return true;
	else if(first == NULL && second != NULL) return false;
	else if(first == NULL && second == NULL) return false;
	return first->getFirstValue() < second->getFirstValue();
}

void RosenbrockFitnessPolicy::setWorstFitness(Solution<>* solution) {
	if(solution != NULL) solution->setFitness(DBL_MAX);
}

void RosenbrockFitnessPolicy::setWorstFitness(Fitness<> *fitness) {
	if(fitness != NULL) *fitness = DBL_MAX;
}

void RosenbrockFitnessPolicy::setBestFitness(Solution<>* solution) {
	if(solution != NULL) solution->setFitness(0.0);
}

void RosenbrockFitnessPolicy::setBestFitness(Fitness<> *fitness) {
	if(fitness != NULL) *fitness = 0.0;
}


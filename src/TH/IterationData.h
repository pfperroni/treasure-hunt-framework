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
 * @file IterationData.h
 * @class IterationData
 * @author Peter Frank Perroni
 * @brief Repository class that stores useful data about the TH iterations.
 */

#ifndef ITERATIONDATA_H_
#define ITERATIONDATA_H_

#include "Solution.h"

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
struct IterationData {
	Solution<P, pSize, F, fSize, V, vSize> **population;
	Solution<P, pSize, F, fSize, V, vSize> *generalBest;
	Solution<P, pSize, F, fSize, V, vSize> *parentBest;
	Solution<P, pSize, F, fSize, V, vSize> *iterationBest;
	int n;
	int populationSize;
	int currTime;
	int currIteration;
	int currNumberEvaluation;
	long maxTimeSeconds;
	long long maxNumberEvaluations;
	long maxIterations;

public:
	/**
	 * @brief Constructor that creates an IterationData instance.
	 * @param population A pointer to the actual population of Solution instances.
	 *        The entire population is cloned and then stored.
	 * @param populationSize The population size.
	 * @param maxTimeSeconds The maximum time (in seconds) allowed to run.
	 * @param maxNumberEvaluations The maximum number of Fitness evaluations allowed.
	 * @param maxIterations The maximum number of TH iterations allowed.
	 */
	IterationData(Solution<P, pSize, F, fSize, V, vSize> **population,
			int populationSize,
			long maxTimeSeconds = 0,
			long long maxNumberEvaluations = 0,
			long maxIterations = 0) {

		if(populationSize <= 0 || population == NULL) {
			throw std::invalid_argument("Population size must be greater than zero.");
		}

		n = population[0]->getNDimensions();
		iterationBest = new Solution<P, pSize, F, fSize, V, vSize>(n);
		parentBest = new Solution<P, pSize, F, fSize, V, vSize>(n);
		generalBest = new Solution<P, pSize, F, fSize, V, vSize>(n);

		this->populationSize = populationSize;
		this->population = new Solution<P, pSize, F, fSize, V, vSize>*[populationSize];
		for(int i=0; i < populationSize; i++){
			this->population[i] = new Solution<P, pSize, F, fSize, V, vSize>(population[i]);
		}

		this->maxTimeSeconds = maxTimeSeconds;
		this->maxNumberEvaluations = maxNumberEvaluations;
		this->maxIterations = maxIterations;
		currTime = 0;
		currIteration = 0;
		currNumberEvaluation = 0;
	}
	~IterationData() {
		for(int i=0; i < populationSize; i++){
			delete population[i];
		}
		delete population;
		delete iterationBest;
		delete generalBest;
		delete parentBest;
	}

	/**
	 * @brief Copy the contents of the general best solution to an internal clone.
	 * @param generalBest The current general best Solution instance.
	 */
	void setGeneralBest(Solution<P, pSize, F, fSize, V, vSize> *generalBest) {
		if(generalBest != NULL) *this->generalBest = generalBest;
	}

	Solution<P, pSize, F, fSize, V, vSize>* getGeneralBest() {
		return generalBest;
	}

	/**
	 * @brief Copy the contents of the all population individuals to their internal clones.
	 * @param population The population at current state.
	 * @param populationSize The population size.
	 */
	void setPopulation(Solution<P, pSize, F, fSize, V, vSize> **population, int populationSize) {
		if(populationSize > this->populationSize) {
			throw std::invalid_argument(std::string("Invalid population size [ ")
					+ std::to_string(populationSize) + " > " + std::to_string(this->populationSize) + "].");
		}

		for(int i=0; i < populationSize; i++){
			*this->population[i] = population[i];
		}
	}

	Solution<P, pSize, F, fSize, V, vSize>** getPopulation() {
		return population;
	}

	int getPopulationSize() {
		return populationSize;
	}

	/**
	 * @brief Set the current running time of the current TH instance.
	 * @param currTime The current running time of the current TH instance.
	 */
	void setCurrTime(int currTime) {
		this->currTime = currTime;
	}

	int getCurrTime() {
		return currTime;
	}

	/**
	 * @brief Set the current number of TH iterations for current TH instance.
	 * @param currIteration The current number of TH iterations for current TH instance.
	 */
	void setCurrIteration(int currIteration) {
		this->currIteration = currIteration;
	}

	int getCurrIteration() {
		return currIteration;
	}

	/**
	 * @brief Set the current number of fitness evaluations for current TH instance.
	 * @param currNumberEvaluation The current number of fitness evaluations for current TH instance.
	 */
	void setCurrNumberEvaluation(int currNumberEvaluation) {
		this->currNumberEvaluation = currNumberEvaluation;
	}

	int getCurrNumberEvaluation() {
		return currNumberEvaluation;
	}

	long getMaxIterations() {
		return maxIterations;
	}

	long long getMaxNumberEvaluations() {
		return maxNumberEvaluations;
	}

	long getMaxTimeSeconds() {
		return maxTimeSeconds;
	}

	/**
	 * @brief Obtains the percentage of running time that has already been spent,
	 *        according to the stopping criteria configured.
	 * @return The percentage of running time that has already been spent,
	 *         in the interval [0, 1].
	 */
	double percentageRuntime(){
		double perc2, perc = 0;
		if(maxNumberEvaluations > 0) {
			perc = currNumberEvaluation / (double)maxNumberEvaluations;
		}
		if(maxIterations > 0) {
			perc2 = currIteration / (double)maxIterations;
			if(perc2 > perc) perc = perc2;
		}
		if(maxTimeSeconds > 0){
			perc2 = currTime / (double)maxTimeSeconds;
			if(perc2 > perc) perc = perc2;
		}
		return perc;
	}

	Solution<P, pSize, F, fSize, V, vSize>* getParentBest() {
		return parentBest;
	}

	/**
	 * @brief Copy the contents of the parent best solution to an internal clone.
	 * @param parentBest The current parent best Solution instance.
	 */
	void setParentBest(Solution<P, pSize, F, fSize, V, vSize> *parentBest) {
		*this->parentBest = parentBest;
	}

	Solution<P, pSize, F, fSize, V, vSize>* getIterationBest() {
		return iterationBest;
	}

	/**
	 * @brief Copy the contents of the iteration's best solution to an internal clone.
	 * @param iterationBest The current iteration's best Solution instance.
	 */
	void setIterationBest(Solution<P, pSize, F, fSize, V, vSize> *iterationBest) {
		*this->iterationBest = iterationBest;
	}

	/**
	 * @brief Get the number of dimensions of current optimization problem.
	 * @return The number of dimensions of current optimization problem.
	 */
	int getNDimensions(){
		return n;
	}
};

#endif /* ITERATIONDATA_H_ */

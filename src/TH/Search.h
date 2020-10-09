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
 * @file Search.h
 * @class Search
 * @author Peter Frank Perroni
 * @brief Template for optimization methods.
 * @details TH uses the ConvergenceControlPolicy to run any optimization method,
 *          which in turn calls an instance of this Search template.
 *
 *          Therefore, the only requirement to integrate any optimization algorithm
 *          with TH is to extend this Search template, using its methods accordingly.
 */

#ifndef SEARCH_H_
#define SEARCH_H_

#include "FitnessPolicy.h"
#include "SearchSpace.h"

#include <mpi.h>

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class Search {
	FitnessPolicy<P, pSize, F, fSize, V, vSize> *fitnessPolicy;
	Solution<P, pSize, F, fSize, V, vSize> **population;
	SearchSpace<P> *searchSpace;
	int preferredPopulationSize;
	int populationSize;

protected:
	Solution<P, pSize, F, fSize, V, vSize>** getPopulation(){
		return population;
	}

public:
	/**
	 * @brief Constructor to create a Search instance.
	 * @param preferredPopulationSize The population size expected for the search algorithm.
	 */
	Search(int preferredPopulationSize) {
		this->preferredPopulationSize = preferredPopulationSize;
		fitnessPolicy = NULL;
		population =  NULL;
		searchSpace = NULL;
		populationSize = 0;
	}
	virtual ~Search() {}

	/**
	 * @brief Set the population.
	 *
	 * The population set is already initialized and ready to be optimized.
	 *
	 * Notice the populationSize can be different from the preferredPopulationSize, since
	 * the actual population size is the max(preferredPopulationSize) for all search algorithms
	 * enabled for the current TH optimization run.
	 *
	 * @param population The actual population.
	 * @param populationSize The number of individuals in the population.
	 */
	void setPopulation(Solution<P, pSize, F, fSize, V, vSize> **population, int populationSize){
		this->population = population;
		this->populationSize = populationSize;
	}

	int getPreferredPopulationSize() {
		return preferredPopulationSize;
	}

	int getPopulationSize() {
		return populationSize;
	}

	/**
	 * @brief Set the fitness policy to evaluate the current problem under optimization.
	 * @param fitnessPolicy The fitness policy to evaluate every Solution instance.
	 */
	void setFitnessPolicy(FitnessPolicy<P, pSize, F, fSize, V, vSize> *fitnessPolicy) {
		this->fitnessPolicy = fitnessPolicy;
	}

	FitnessPolicy<P, pSize, F, fSize, V, vSize>* getFitnessPolicy(){
		return fitnessPolicy;
	}

	SearchSpace<P>* getSearchSpace() {
		return searchSpace;
	}

	/**
	 * @brief Set the full search space.
	 *
	 * Notice that at this point the population has already been initialized,
	 * and therefore, the "anchor" sub-region is not required at this point.
	 *
	 * @param searchSpace The full search space.
	 */
	void setSearchSpace(SearchSpace<P>* searchSpace) {
		this->searchSpace = searchSpace;
	}

	/**
	 * @brief Method responsible for setting up all the optimization algorithm
	 *        for the next optimization.
	 *
	 * All details required to prepare the search algorithms for the next optimization
	 * should be done in this method, like for example reseting all counters, getting
	 * the next population (and its size), the next fitness policy, the next random seeds,
	 * pre-evaluating the fitness of the starting population, etc.
	 */
	virtual void startup()=0;

	/**
	 * @brief Method responsible for all post-processing after the optimization has completed.
	 *
	 * If the optimization method requires a post-processing phase so that a single best
	 * result can be returned to the search group, such post-processing should be done in
	 * this method.
	 */
	virtual void finalize()=0;

	/**
	 * @brief Method responsible for executing the actual optimization to obtain the
	 *        next best result.
	 *
	 * The fitness evaluations must be done by calling the method
	 * {@link FitnessPolicy::apply(Solution<P, pSize, F, fSize, V, vSize>*)},
	 * and any comparison between Solutions should be done by calling the method
	 * {@link FitnessPolicy::firstIsBetter(Solution<P, pSize, F, fSize, V, vSize>*, Solution<P, pSize, F, fSize, V, vSize>*)}
	 * or {@link FitnessPolicy::firstIsBetter(Fitness<F, fSize>*, Fitness<F, fSize>*)}.
	 *
	 * Given that the Search execution time is managed by the ConvergenceControlPolicy,
	 * this method should keep running only long enough (up to M evaluations) to obtain
	 * a result better than the previous "best" result, so that improvement data can be
	 * collected and the convergence stagnation management can work properly.
	 *
	 * Notice that the best result for the starting population should be evaluated at
	 * the startup() method.
	 *
	 * @param M The maximum number of evaluations allowed to obtain the next improvement.
	 */
	virtual void next(int M)=0;

	/**
	 * @brief Inform the ConvergenceControlPolicy that no next improvement could be found
	 *        in reasonable time.
	 *
	 * If the optimization algorithm has its own efficient mechanism to detect stagnation
	 * quickly, it could use this method to inform the ConvergenceControlPolicy about such
	 * stagnation.
	 *
	 * Slow or complex stagnation management mechanisms are not recommended on TH because
	 * it would slow down the search space sampling and communication between TH instances.
	 *
	 * @return True if the optimization algorithm has detected a strong stagnation.
	 *         False otherwise.
	 */
	virtual bool isStuck()=0;

	/**
	 * @brief Get the best Solution found since the last time the method startup() was called.
	 *
	 * Notice that this best individual should be reset every time the method startup() is called,
	 * preferably using the initial population.
	 *
	 * @return The best Solution found since the last time the method startup() was called.
	 */
	virtual Solution<P, pSize, F, fSize, V, vSize>* getBestIndividual()=0;

	/**
	 * @brief Get the number of FitnessPolicy evaluations performed since the last time the
	 *        method startup() was called.
	 *
	 * Notice that this count should be reset every time the method startup() is called,
	 * preferably considering the number of evaluations required to pre-evaluate the initial
	 * population.
	 *
	 * @return The number of fitness evaluations performed since the last time the method
	 *         startup() was called.
	 */
	virtual int getCurrentNEvals()=0;

	/**
	 * @brief Get the Fitness for the current best Solution.
	 * @return The Fitness for the current best Solution.
	 */
	virtual Fitness<F, fSize>* getBestFitness()=0;

	/**
	 * @brief Get the Fitness for the current best Solution.
	 * @param fitness The destination buffer where the fitness values will be copied.
	 */
	virtual void getBestFitness(F* fitness)=0;

	/**
	 * @brief Get the name of the optimization method implemented.
	 *
	 * This method is used for tracking purpose and it is recommended to set it
	 * accordingly in every optimization method that implements this template.
	 *
	 * @return The name of the optimization method implemented.
	 */
	virtual const char* getName()=0;
};

#endif /* SEARCH_H_ */

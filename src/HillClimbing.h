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
 * @file HillClimbing.h
 * @class HillClimbing
 * @author Peter Frank Perroni
 * @brief Implementation of the classic Hill Climbing optimization algorithm for TH.
 * @details Any implementation of the Search template must split the optimization logic in 3 sections:
 *          - {@link startup()}: initialize the algorithm for a new optimization.
 *          - {@link next()}: perform the actual optimization only until the next improvement.
 *          - {@link finalize()}: perform the post-optimization process, if required.
 */

#ifndef HILLCLIMBING_H_
#define HILLCLIMBING_H_

#include "TH/Search.h"

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class HillClimbing : public Search<P, pSize, F, fSize, V, vSize> {
	Solution<P, pSize, F, fSize, V, vSize> **population;
	FitnessPolicy<P, pSize, F, fSize, V, vSize>* fitnessPolicy;

	unsigned int seed;
	int nEvals, gb, p, n;
	double percMove, step;
	bool stuck;

public:
	HillClimbing(double percMove, double step, int populationSize) : Search<P, pSize, F, fSize, V, vSize>(populationSize) {
		this->percMove = percMove;
		this->step = step;

		population = NULL;
		fitnessPolicy = NULL;

		seed = 1;
		nEvals = 0;
		p = 0;
		n = 0;
		gb = -1;
		stuck = false;
	}
	~HillClimbing() {}

	/**
	 * @brief Initialize the algorithm for a new optimization.
	 */
	void startup() {
		p = this->getPopulationSize();
		if(p <= 0) {
			throw std::invalid_argument("The population size must be greater than zero.");
		}
		population = this->getPopulation();
		fitnessPolicy = this->getFitnessPolicy();
		seed = THUtil::getRandomSeed();
		n = this->getSearchSpace()->getNDimensions();
		nEvals = 0;
		gb = 0;
		stuck = false;

		for(int i=1; i < p; i++){
			if(fitnessPolicy->firstIsBetter(population[i], population[gb])){
				gb = i;
			}
		}
	}

	/**
	 * @brief Perform the post-optimization process, if required.
	 */
	void finalize(){}

	/**
	 * @brief Perform the actual optimization only until the next improvement.
	 */
	void next(int M){
		Solution<P, pSize, F, fSize, V, vSize> candidate(n);
		SearchSpace<P>* searchSpace = this->getSearchSpace();
		Dimension<P> *dim;
		int i, d, noImprove = 0;
		bool found = false;
		while((!found && noImprove < MAX_NO_IMPROVE) && nEvals < M){
			for(i=0; i < p && nEvals < M; i++){
				for(d=0; d < n && nEvals < M; d++){
					if(THUtil::randUniformDouble(seed, 0, 1) > percMove) continue;
					candidate = population[i]; // Copy the solution.
					dim = searchSpace->getOriginalDimension(d);
					candidate[d]->sum(step * THUtil::randUniformDouble(seed, dim->getStartPoint(), dim->getEndPoint()));
					candidate[d]->adjustUpperBound(dim->getEndPoint());
					candidate[d]->adjustLowerBound(dim->getStartPoint());
					fitnessPolicy->apply(&candidate);
					nEvals++;
					if(fitnessPolicy->firstIsBetter(&candidate, population[i])) {
						*(*population[i])[d] = candidate[d];
						if(i != gb && fitnessPolicy->firstIsBetter(population[i], population[gb])){
							found = true;
							gb = i;
						}
					}
					else *candidate[d] = (*population[i])[d];
				}
			}
			if(!found) noImprove++;
		}
		if(noImprove == MAX_NO_IMPROVE) stuck = true;
	}

	bool isStuck(){return stuck;}

	int getBestPos(){return gb;}

	int getCurrentNEvals(){return nEvals;}

	Solution<P, pSize, F, fSize, V, vSize>* getBestIndividual() {
		return this->getPopulation()[gb];
	}

	Fitness<F, fSize>* getBestFitness(){
		return getBestIndividual()->getFitness();
	}

	void getBestFitness(F* fitness){
		getBestIndividual()->getFitness(fitness);
	}

	const char *getName(){return "HillClimbing";}
};

#endif /* HILLCLIMBING_H_ */

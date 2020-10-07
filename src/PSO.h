/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file PSO.h
 * @class PSO
 * @author Peter Frank Perroni
 * @brief Implementation of the classic Particle Swarm Optimization algorithm for TH.
 * @details Any implementation of the Search template must split the optimization logic in 3 sections:
 *          - {@link startup()}: initialize the algorithm for a new optimization.
 *          - {@link next()}: perform the actual optimization only until the next improvement.
 *          - {@link finalize()}: perform the post-optimization process, if required.
 */

#ifndef PSO_HPP_
#define PSO_HPP_

#include "TH/Search.h"

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class PSO : public Search<P, pSize, F, fSize, V, vSize> {
	Solution<P, pSize, F, fSize, V, vSize> **population;
	FitnessPolicy<P, pSize, F, fSize, V, vSize>* fitnessPolicy;
	Solution<P, pSize, F, fSize, V, vSize> **pBest;
	Solution<P, pSize, F, fSize, V, vSize> **v;

	unsigned int seed;
	int nEvals, gb, p, n;
	double w, c1, c2;
	bool stuck;

public:
	PSO(double w, double c1, double c2, int populationSize) : Search<P, pSize, F, fSize, V, vSize>(populationSize) {
		this->w = w;
		this->c1 = c1;
		this->c2 = c2;

		population = NULL;
		fitnessPolicy = NULL;
		pBest = NULL;
		v = NULL;

		seed = 1;
		nEvals = 0;
		p = 0;
		n = 0;
		gb = -1;
		stuck = false;
	}
	~PSO() {
		if(pBest != NULL){
			for(int i=0; i < p; i++) {
				delete pBest[i];
			}
			delete pBest;

			for(int i=0; i < p; i++){
				delete v[i];
			}
			delete v;
		}
	}

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
		p = this->getPopulationSize();
		n = this->getSearchSpace()->getNDimensions();
		nEvals = 0;
		gb = 0;
		stuck = false;

		if(pBest == NULL){
			pBest = new Solution<P, pSize, F, fSize, V, vSize>*[p];
			for(int i=0; i < p; i++) {
				pBest[i] = new Solution<P, pSize, F, fSize, V, vSize>(n);
			}
		}
		if(v == NULL) {
			v = new Solution<P, pSize, F, fSize, V, vSize>*[p];
			for(int i=0; i < p; i++){
				v[i] = new Solution<P, pSize, F, fSize, V, vSize>(n);
			}
		}

		for(int j, i=0; i < p; i++){
			for(j=0; j < n; j++) *(*v[i])[j] = THUtil::randUniformDouble(seed, 0, 1);
			*pBest[i] = population[i];
			if(i != gb && fitnessPolicy->firstIsBetter(population[i], population[gb])){
				*pBest[i] = population[i];
				gb = i;
			}
		}
	}

	/**
	 * @brief Perform the post-optimization process, if required.
	 */
	void finalize() {
		// Override the population with the respective personal best solutions.
		for(int i=0; i < p; i++){
			*population[i] = pBest[i];
		}
	}

	/**
	 * @brief Perform the actual optimization only until the next improvement.
	 */
	void next(int M) {
		SearchSpace<P>* searchSpace = this->getSearchSpace();
		Dimension<P> *dim;
		Position<P, pSize> pos1, pos2;
		bool found = false;
		int i, j, noImprove = 0;
		double currW = w - (w / M) * nEvals;
		while(!found && nEvals < M && noImprove < MAX_NO_IMPROVE){
			for(i=0; i < p; i++){
				for(j=0; j < n; j++){
					dim = searchSpace->getOriginalDimension(j);
					// c1 * RAND_DOUBLE(seed, 0, 1) * (pb[i][j] - G[i][j])
					pos1 = (*pBest[i])[j];
					pos1.sub((*population[i])[j]);
					pos1.mult(c1 * THUtil::randUniformDouble(seed, 0, 1));
					// c2 * RAND_DOUBLE(seed, 0, 1) * (Gb[j] - G[i][j])
					pos2 = (*population[gb])[j];
					pos2.sub((*population[i])[j]);
					pos2.mult(c2 * THUtil::randUniformDouble(seed, 0, 1));

					// v[i][j] = currW * v[i][j] + c1 * RAND_DOUBLE(seed, 0, 1) * (pb[i][j] - G[i][j])
					//							 + c2 * RAND_DOUBLE(seed, 0, 1) * (Gb[j] - G[i][j]);
					pos2.sum(pos1);

					pos1 = (*v[i])[j];
					pos1.mult(currW);
					pos1.sum(pos2);

   					*(*v[i])[j] = pos1;
					(*population[i])[j]->sum((*v[i])[j]);
					(*population[i])[j]->adjustUpperBound(dim->getEndPoint());
					(*population[i])[j]->adjustLowerBound(dim->getStartPoint());
				}
				fitnessPolicy->apply(population[i]);
				nEvals++;
			}
			for(i=0; i < p; i++){
				if(fitnessPolicy->firstIsBetter(population[i], pBest[i])) {
					*population[i] = pBest[i];
					if(i != gb && fitnessPolicy->firstIsBetter(population[i], population[gb])){
						found = true;
						gb = i;
					}
				}
			}
			if(!found) noImprove++;
			currW -= w / M;
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

	const char *getName(){return "PSO";}
};

#endif /* PSO_HPP_ */

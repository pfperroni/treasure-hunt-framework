/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file BetaRelocationStrategyPolicy.h
 * @class BetaRelocationStrategyPolicy
 * @author Peter Frank Perroni
 * @brief This policy relocates the population based on the
 *        Beta-distribution strategy.
 * @note For the Iterative Partitioning method used, please refer (and cite) to
 *       the following paper:\n
 *   <em>PERRONI, Peter Frank; WEINGAERTNER, Daniel; DELGADO, Myriam Regattieri.
 *   Automated iterative partitioning for cooperatively coevolving particle swarms
 *   in large scale optimization. In: 2015 Brazilian Conference on Intelligent
 *   Systems (BRACIS). IEEE, 2015. p. 19-24.</em>
 */

#ifndef BETARELOCATIONSTRATEGYPOLICY_H_
#define BETARELOCATIONSTRATEGYPOLICY_H_

#include <boost/math/distributions.hpp>
#include "RelocationStrategyPolicy.h"

/**
 * @brief Iterative Partitioning Boost functions.
 */
#define BOOST_L(Br, step) (-Br * step + Br)
#define BOOST_S(Br, step) (Br / (1.0 + exp(12 * Br * step - 6 * Br)))
#define BOOST_E(Br, step) (Br / exp(12 * Br * step))
#define MAX_K(step, Br, boostFunc) min(boostFunc=='L' ? BOOST_L(Br, step) : (boostFunc=='S' ? BOOST_S(Br, step) : BOOST_E(Br, step)), 1.0)

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class BetaRelocationStrategyPolicy : public RelocationStrategyPolicy<P, pSize, F, fSize, V, vSize> {

	unsigned int seed;
	int maxTries, nTries;
	float K, maxK;
	double prevBestFitness;
	char diplacementType, boostInc, boostType;
	bool firstPass;

	/**
	 * @brief Iterative Partitioning default configuration.
	 */
	void configIPDisplacementType() {
		boostType = 'E';
		boostInc= 1;
		maxTries = 3;
	}

	/**
	 * @brief Iterative Partitioning calculation.
	 */
	float getAttraction(double step, double currGb, double prevGb){
		float Kr = 1.0f / maxTries;
		if(K <= 0){
			K = maxK = MAX_K(0.0, boostInc, boostType);
		}
		else if(1 - currGb/prevGb < 5e-5){
			if((int)(K*1e4) <= (int)(maxK*Kr*1e4)){
				if(++nTries == maxTries){
					maxK = MAX_K(0.0, boostInc, boostType);
					nTries = 0;
				}
				else maxK = MAX_K(step, boostInc, boostType);
				K = maxK;
			}
			else K = K - maxK*Kr;
			if(K < 1e-30) getAttraction(step, currGb, prevGb);
		}
		else nTries = 0;
		return K;
	}

	inline double getPrevBestFitness(double bestFit) {
		return (firstPass) ? (firstPass=false)+(prevBestFitness=bestFit) : (prevBestFitness=bestFit);
	}

public:
	BetaRelocationStrategyPolicy() {
		seed = THUtil::getRandomSeed();
		diplacementType = 'L';
		configIPDisplacementType();
		maxTries = 0;
		nTries = 0;
		K = -1;
		prevBestFitness = 0;
		firstPass = true;
	}
	~BetaRelocationStrategyPolicy() {}

	/**
	 * @brief This method implements the policy to relocate TH instance's population
	 * at every TH instance's iteration, based on the Beta-distribution strategy.
	 *
	 * @param relocationStrategyData The repository containing useful data to perform the relocation.
	 * @param region The "anchor" Region for current TH instance.
	 * @param population The population, which is comprised of a list of solutions.
	 * @param populationSize The population size.
	 */
	void apply(RelocationStrategyData<P, pSize, F, fSize, V, vSize> *relocationStrategyData,
			Region<P> *region,
			Solution<P, pSize, F, fSize, V, vSize> **population,
			int populationSize){
		if(relocationStrategyData == NULL || region == NULL || population == NULL || populationSize == 0)  {
			throw std::invalid_argument("All parameters for relocation strategy must be provided.");
		}

		IterationData<P, pSize, F, fSize, V, vSize> *iterationData = relocationStrategyData->getIterationData();

		// Convert to Beta strategy.
		BetaRelocationStrategyData<P, pSize, F, fSize, V, vSize> *betaRelocationStrategyData =
				(BetaRelocationStrategyData<P, pSize, F, fSize, V, vSize>*)relocationStrategyData;

		// Calculate the beta displacement rate.
		double displacementRate;
		if(diplacementType == 'L') {
			displacementRate = iterationData->percentageRuntime();
		}
		else {
			double bestFit = iterationData->getGeneralBest()->getFitness()->getFirstValue();
			displacementRate = getAttraction(iterationData->percentageRuntime(), bestFit, getPrevBestFitness(bestFit));
		}
		betaRelocationStrategyData->setDisplacementRate(displacementRate);

		// Calculate the beta PDF.
		double betaProb = betaRelocationStrategyData->getBetaStartingPerc() * betaRelocationStrategyData->getBetaMax()
				* pow(max(betaRelocationStrategyData->getDisplacementRate(), 1e-5), betaRelocationStrategyData->getBetaAccelerationCoef());
		boost::math::beta_distribution<> distrib(betaRelocationStrategyData->getBetaMax()-betaProb, betaProb);

		// Relocate the population members that were not repositioned on previous processes.
		Position<P, pSize> pos, tmp;
		Dimension<P> *dim;
		map<int, Dimension<P>*> *originalDimensions = region->getOriginalDimensions();
		Solution<P, pSize, F, fSize, V, vSize> *parentBest = iterationData->getParentBest();
		int n = population[0]->getNDimensions();
		for(int j, i=0; i < populationSize; i++){
			population[i]->reset(region); // Obtain a new position inside TH instance's sub-region for this population member.
			// Relocate this individual according to the beta strategy toward the parent best.
			for(j=0; j < n; j++){
				dim = (*originalDimensions)[j];
				pos = (*population[i])[j];
				tmp = pos;
				tmp.sub((*parentBest)[j]);
				tmp.mult(quantile(distrib, THUtil::randUniformDouble(seed, 0, 1)));
				pos.sub(tmp);
				pos.adjustUpperBound(dim->getEndPoint());
				pos.adjustLowerBound(dim->getStartPoint());
				*(*population[i])[j] = pos; //quantile(distrib, THUtil::randUniformDouble(seed, 0, 1)) * (pos - parentBest[j]);
			}
		}
	}

	void setIPDisplacementType() {
		configIPDisplacementType();
		diplacementType = 'I';
	}

	void setIPDisplacementType(char boostType, double boostInc, int nTries) {
		this->boostType = boostType;
		this->boostInc= boostInc;
		this->maxTries = nTries;
		diplacementType = 'I';
	}

	void setLinearDisplacementType() {
		diplacementType = 'L';
	}
};

#endif /* BETARELOCATIONSTRATEGYPOLICY_H_ */

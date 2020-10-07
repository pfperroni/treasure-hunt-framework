/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file BetaRelocationStrategyData.h
 * @class BetaRelocationStrategyData
 * @author Peter Frank Perroni
 * @brief This class is a repository containing useful data
 *        to perform the Beta-relocation strategy.
 */

#ifndef BETARELOCATIONSTRATEGYDATA_H_
#define BETARELOCATIONSTRATEGYDATA_H_

#include "RelocationStrategyData.h"

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
struct BetaRelocationStrategyData : public RelocationStrategyData<P, pSize, F, fSize, V, vSize> {
	double betaStartingPerc;
	double betaMax;
	double displacementRate;
	double betaAccelerationCoef;

public:
	BetaRelocationStrategyData(double betaStartingPerc, double betaMax, double displacementRate, double betaAccelerationCoef) {
		this->betaStartingPerc= betaStartingPerc;
		this->betaMax = betaMax;
		this->displacementRate = displacementRate;
		this->betaAccelerationCoef = betaAccelerationCoef;
	}
	~BetaRelocationStrategyData() {}

	double getBetaAccelerationCoef() {
		return betaAccelerationCoef;
	}

	void setBetaAccelerationCoef(double betaAccelerationCoef) {
		this->betaAccelerationCoef = betaAccelerationCoef;
	}

	double getBetaMax() {
		return betaMax;
	}

	void setBetaMax(double betaMax) {
		this->betaMax = betaMax;
	}

	double getBetaStartingPerc() {
		return betaStartingPerc;
	}

	void setBetaStartingPerc(double betaStartingPerc) {
		this->betaStartingPerc = betaStartingPerc;
	}

	double getDisplacementRate() {
		return displacementRate;
	}

	void setDisplacementRate(double displacementRate) {
		this->displacementRate = displacementRate;
	}
};

#endif /* BETARELOCATIONSTRATEGYDATA_H_ */

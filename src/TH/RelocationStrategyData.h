/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file RelocationStrategyData.h
 * @class RelocationStrategyData
 * @author Peter Frank Perroni
 * @brief Repository class that stores useful data required for
 *        the population's relocation strategy.
 */

#ifndef RELOCATIONSTRATEGYDATA_H_
#define RELOCATIONSTRATEGYDATA_H_

#include "IterationData.h"

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
struct RelocationStrategyData {
	IterationData<P, pSize, F, fSize, V, vSize> *iterationData;

public:
	RelocationStrategyData() {
		iterationData = NULL;
	}
	~RelocationStrategyData() {}

	/**
	 * @brief Set the IterationData instance that stores useful data about the TH iterations.
	 * @param iterationData The IterationData instance.
	 */
	void setIterationData(IterationData<P, pSize, F, fSize, V, vSize> *iterationData) {
		this->iterationData = iterationData;
	}

	IterationData<P, pSize, F, fSize, V, vSize>* getIterationData() {
		return iterationData;
	}
};

#endif /* RELOCATIONSTRATEGYDATA_H_ */

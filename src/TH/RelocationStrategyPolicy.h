/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file RelocationStrategyPolicy.h
 * @class RelocationStrategyPolicy
 * @author Peter Frank Perroni
 * @brief Template for the policy that relocates the population
 *        at every TH instance's iteration.
 */

#ifndef RELOCATIONSTRATEGYPOLICY_H_
#define RELOCATIONSTRATEGYPOLICY_H_

#include "RelocationStrategyData.h"

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class RelocationStrategyPolicy {
public:
	RelocationStrategyPolicy() {}
	virtual ~RelocationStrategyPolicy() {}

	/**
	 * @brief Virtual method to apply the policy that will relocate TH instance's population
	 * at every TH instance's iteration.
	 *
	 * A method implementing this virtual method is the responsible for updating
	 * the location in the search space of every population individual.
	 * It should use the Region (a.k.a. the "anchor") as base to calculate the relocation,
	 * as well as the {@link Region::getOriginalDimensions()} to limit the overall
	 * search space boundaries.
	 *
	 * @param relocationStrategyData The repository containing useful data to perform the relocation.
	 * @param region The "anchor" Region for current TH instance.
	 * @param population The population, which is comprised of a list of solutions.
	 * @param populationSize The population size.
	 */
	virtual void apply(RelocationStrategyData<P, pSize, F, fSize, V, vSize> *relocationStrategyData,
			Region<P> *region,
			Solution<P, pSize, F, fSize, V, vSize> **population,
			int populationSize) = 0;
};

#endif /* RELOCATIONSTRATEGYPOLICY_H_ */

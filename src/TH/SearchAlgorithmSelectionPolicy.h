/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file SearchAlgorithmSelectionPolicy.h
 * @class SearchAlgorithmSelectionPolicy
 * @author Peter Frank Perroni
 * @brief Template to select the next optimization method to be executed and rank it after execution.
 * @details TH's search group uses this template to obtain the next Search instance to be called,
 *          allowing optimization by multiple search algorithms (round-robin, random, combinations,
 *          hyper-heuristics, etc).
 *          Once the Search instance has completed its optimization, the search group also uses
 *          this template to rank the performance of the Search instance just executed.
 */

#ifndef SEARCHALGORITHMSELECTIONPOLICY_H_
#define SEARCHALGORITHMSELECTIONPOLICY_H_

#include "SearchScore.h"
#include "THTree.h"

#include <vector>

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class SearchAlgorithmSelectionPolicy {

public:
	SearchAlgorithmSelectionPolicy() {}
	virtual ~SearchAlgorithmSelectionPolicy() {}

	/**
	 * @brief Choose the next optimization algorithm to be executed.
	 * @param ID The TH instance unique identifier.
	 * @param tree The THTree topology where this TH instance is running.
	 * @param algorithms The list of SearchScore instances containing all optimization
	 *        algorithms enabled and their respective scores.
	 * @return The next Search instance to be called.
	 */
	virtual Search<P, pSize, F, fSize, V, vSize>* apply(
			int ID, THTree *tree, vector<SearchScore<P, pSize, F, fSize, V, vSize>*>* algorithms) = 0;

	/**
	 * @brief Rank the performance of the Search instance previously executed.
	 * @param ID The TH instance unique identifier.
	 * @param tree The THTree topology where this TH instance is running.
	 * @param algorithms The list of SearchScore instances containing all optimization
	 *        algorithms enabled and their respective scores.
	 * @param currAlg The Search instance under evaluation.
	 * @param currFitness The best Fitness obtained by currAlg on current run.
	 * @param currentNEvals The number of evaluations performed by currAlg to obtain currFitness.
	 * @param nEvals The total number of evaluations since the TH instance started.
	 */
	virtual void rank(
			int ID, THTree *tree, vector<SearchScore<P, pSize, F, fSize, V, vSize>*>* algorithms,
			Search<P, pSize, F, fSize, V, vSize>* currAlg, Fitness<F, fSize> *currFitness,
			int currentNEvals, long long nEvals
	) = 0;
};

#endif /* SEARCHALGORITHMSELECTIONPOLICY_H_ */

/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file ConvergenceControlPolicy.h
 * @class ConvergenceControlPolicy
 * @author Peter Frank Perroni
 * @brief Template for the policy that runs, monitors and controls
 *        the convergence for the current TH iteration.
 */

#ifndef CONVERGENCECONTROLPOLICY_H_
#define CONVERGENCECONTROLPOLICY_H_

#include "Search.h"

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class ConvergenceControlPolicy {
	int budgetSize;

public:
	/**
	 * @brief Constructor to setup the maximum number of fitness function evaluations allowed.
	 * @param budgetSize The maximum number of fitness function evaluations allowed.
	 */
	ConvergenceControlPolicy(int budgetSize) {
		this->budgetSize = budgetSize;
	}
	virtual ~ConvergenceControlPolicy() {}

	/**
	 * @brief Virtual method that will run, monitor and control one TH iteration.
	 *
	 * A method implementing this virtual method is the responsible for calling
	 * sequential iterations of the actual optimization method, through the call of
	 * {@link Search::next(int)}.
	 *
	 * @param search The optimization method.
	 */
	virtual void run(Search<P, pSize, F, fSize, V, vSize> *search) = 0;

	/**
	 * @brief Get the maximum number of fitness function evaluations allowed.
	 */
	int getBudgetSize() { return budgetSize; }
};

#endif /* CONVERGENCECONTROLPOLICY_H_ */

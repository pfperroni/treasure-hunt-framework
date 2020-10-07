/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file FitnessPolicy.h
 * @class FitnessPolicy
 * @author Peter Frank Perroni
 * @brief Template for the policy that calculates the fitness (or cost)
 *        for the problem under optimization.
 * @details In TH, every problem must have its own implementation of FitnessPolicy,
 *          since TH trust in this single class to provide all details about the
 *          problem under optimization.
 */

#ifndef FITNESSPOLICY_H_
#define FITNESSPOLICY_H_

#include "Solution.h"
#include "THTree.h"

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class FitnessPolicy {
public:
	FitnessPolicy(){}
	virtual ~FitnessPolicy(){}

	/**
	 * @brief This method calculates the fitness for the Solution instance provided.
	 *
	 * All processes required to calculate the fitness for a Solution instance
	 * must be performed in this single method call.
	 *
	 * @param solution The Solution instance to be evaluated.
	 */
	virtual void apply(Solution<P, pSize, F, fSize, V, vSize> *solution) = 0;

	/**
	 * @brief Check if the first Solution is better than the second Solution.
	 *
	 * Given that only the FitnessPolicy actually knows the problem being optimized,
	 * this method should tell TH framework which from any two Solutions is better.
	 *
	 * @param first The first Solution instance to compare.
	 * @param second The second Solution instance to compare.
	 * @return True if the first Solution is better than the second Solution.
	 *         False otherwise.
	 */
	virtual bool firstIsBetter(Solution<P, pSize, F, fSize, V, vSize> *first,
								Solution<P, pSize, F, fSize, V, vSize> *second) = 0;

	/**
	 * @brief Check if the first Fitness is better than the second Fitness.
	 *
	 * Given that only the FitnessPolicy actually knows the problem being optimized,
	 * this method should tell TH framework which from any two Fitness is better.
	 *
	 * @param first The first Fitness instance to compare.
	 * @param second The second Fitness instance to compare.
	 * @return True if the first Fitness is better than the second Fitness.
	 *         False otherwise.
	 */
	virtual bool firstIsBetter(Fitness<F, fSize> *first, Fitness<F, fSize> *second) = 0;

	/**
	 * @brief Set the Solution instance's Fitness to the worst estimated fitness.
	 * @param solution The Solution instance to set the Fitness.
	 */
	virtual void setWorstFitness(Solution<P, pSize, F, fSize, V, vSize> *solution) = 0;

	/**
	 * @brief Set the Fitness instance to the worst estimated fitness.
	 * @param fitness The Fitness instance to set.
	 */
	virtual void setWorstFitness(Fitness<F, fSize> *fitness) = 0;

	/**
	 * @brief Set the Solution instance's Fitness to the best estimated fitness.
	 * @param solution The Solution instance to set the Fitness.
	 */
	virtual void setBestFitness(Solution<P, pSize, F, fSize, V, vSize> *solution) = 0;

	/**
	 * @brief Set the Fitness instance to the best estimated fitness.
	 * @param fitness The Fitness instance to set.
	 */
	virtual void setBestFitness(Fitness<F, fSize> *fitness) = 0;

	/**
	 * @brief Get the minimum estimated fitness value for the problem being optimized.
	 *
	 * Notice that this method requires a single value as response.
	 * Therefore, if the problem has a fitness composed of multiple values,
	 * such values must be combined into a single value.
	 *
	 * If the minimum single fitness value is unknown, give your best guess (preferred)
	 * or use DBL_MIN (usually not the best choice).
	 *
	 * @return The minimum estimated fitness value for the problem being optimized.
	 */
	virtual double getMinEstimatedFitnessValue() = 0;
};

#endif /* FITNESSPOLICY_H_ */

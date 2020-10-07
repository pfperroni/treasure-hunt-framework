/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file SearchScore.h
 * @class SearchScore
 * @author Peter Frank Perroni
 * @brief This class stores a Search instance and its score.
 * @details TH's search group uses the SearchAlgorithmSelectionPolicy to choose and rank
 *          the optimization methods.
 */

#ifndef SEARCHSCORE_H_
#define SEARCHSCORE_H_

#include "Search.h"

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class SearchScore {
	Search<P, pSize, F, fSize, V, vSize> *searchAlgorithm;
	double weight;
	double score;
	double frequency;
	double deprecation;

public:
	/**
	 * @brief Constructor to create a SearchScore instance.
	 * @param searchAlgorithm The Search instance containing the optimization algorithm.
	 * @param weight The weight of the searchAlgorithm in the scoring process.
	 */
	SearchScore(Search<P, pSize, F, fSize, V, vSize> *searchAlgorithm, double weight = 1){
		this->searchAlgorithm = searchAlgorithm;
		this->weight = weight;
		score = 1;
		frequency = 1;
		deprecation = 1;
	}
	~SearchScore(){}

	double getWeight() { return weight; }

	/**
	 * @brief Set the weight of the search algorithm in the scoring process.
	 *
	 * Avoid calling this method, since it could affect the scoring process.
	 *
	 * @param weight The new weight of the search algorithm in the scoring process.
	 */
	void setWeight(double weight) { this->weight = weight; }

	double getScore() { return score; }

	void setScore(double score) { this->score = score; }

	double getFrequency() { return frequency; }

	/**
	 * @brief Set the number of times this search algorithm has been chosen.
	 *
	 * The class that implements SearchAlgorithmSelectionPolicy could use this method
	 * to track how many times this search algorithm has been chosen (i.e. its "frequency").
	 *
	 * @param frequency The number of times this search algorithm has been chosen.
	 */
	void setFrequency(double frequency) { this->frequency = frequency; }

	double getDeprecation() { return deprecation; }

	/**
	 * @brief Set the number of times this search algorithm has not been chosen.
	 *
	 * The class that implements SearchAlgorithmSelectionPolicy could use this method
	 * to track how long this search algorithm has not been chosen (i.e. its "deprecation").
	 *
	 * @param deprecation The number of times this search algorithm has not been chosen.
	 */
	void setDeprecation(double deprecation) { this->deprecation = deprecation; }

	Search<P, pSize, F, fSize, V, vSize>* getSearchAlgorithm() { return searchAlgorithm; }
};

#endif /* SEARCHSCORE_H_ */

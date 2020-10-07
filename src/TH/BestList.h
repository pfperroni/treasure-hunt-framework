/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file BestList.h
 * @class BestList
 * @author Peter Frank Perroni
 * @brief An instance of this class holds the best-list for its respective TH instance.
 */

#ifndef BESTLIST_H_
#define BESTLIST_H_

#include "Solution.h"

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class BestList {
	Solution<P, pSize, F, fSize, V, vSize> **bestList;
	int listSize;
	int n;

public:
	/**
	 * @brief Constructor that generates an empty best-list instance.
	 * @param listSize List size.
	 * @param n Maximum number of dimensions to be optimized.
	 */
	BestList(int listSize, int n){
		if(listSize <= 0) throw std::invalid_argument("The best list size is invalid.");
		this->listSize = listSize;
		this->n = n;
		bestList = new Solution<P, pSize, F, fSize, V, vSize>*[listSize];
		for(int i=0; i < listSize; i++) bestList[i] = NULL;
	}

	/**
	 * @brief Constructor that generates a new best-list initialized as a full copy of another BestList instance.
	 * @param bestList The source BestList instance.
	 */
	BestList(BestList<P, pSize, F, fSize, V, vSize>* bestList){
		this->listSize = bestList->getListSize();
		if(bestList == NULL || this->listSize == 0) throw std::invalid_argument("The best list size is invalid.");
		this->n = bestList->getNDimensions();
		this->bestList = new Solution<P, pSize, F, fSize, V, vSize>*[listSize];
		Solution<P, pSize, F, fSize, V, vSize> *solution;
		for(int i=0; i < listSize; i++) {
			solution = (*bestList)[i];
			if(solution != NULL) this->bestList[i] = new Solution<P, pSize, F, fSize, V, vSize>(solution);
			else this->bestList[i] = NULL;
		}
	}

	/**
	 * @brief This destructor deletes all Solutions stored in the best-list (i.e. releases their memories).
	 */
	~BestList() {
		for(int i=0; i < listSize; i++){
			if(bestList[i] != NULL) delete bestList[i];
		}
		delete bestList;
	}

	/**
	 * @brief Operator that selects a Solution based on its index in the list of solutions.
	 *
	 * The pointer to the actual object is returned, instead of a simple copy.
	 *
	 * @param idx The position of the Solution in the list (index list starts in zero).
	 * @return A pointer to the Solution selected.
	 */
	Solution<P, pSize, F, fSize, V, vSize>* operator [](int idx){
		if(idx < 0 || idx > listSize) throw std::invalid_argument("The best list index is invalid");
		return bestList[idx];
	}

	/**
	 * @brief Operator to set an element in the list.
	 *
	 * The old element (if any) is deleted (i.e. its memory is released) and the
	 * respective index position will point to the solution received as parameter.
	 *
	 * @param idx The index in the list to set the new Solution (list starts in zero).
	 * @param solution The new Solution instance to add to the list.
	 */
	void set(int idx, Solution<P, pSize, F, fSize, V, vSize> *solution){
		if(idx < 0 || idx > listSize) throw std::invalid_argument("The best list index is invalid");
		if(solution == NULL) throw std::invalid_argument("The solution cannot be empty.");
		if(bestList[idx] != NULL) delete bestList[idx];
		bestList[idx] = solution;
	}

	int getListSize(){
		return listSize;
	}

	int getNDimensions() {
		return n;
	}
};

#endif /* BESTLIST_H_ */

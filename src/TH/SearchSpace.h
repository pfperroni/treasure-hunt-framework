/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file SearchSpace.h
 * @class SearchSpace
 * @author Peter Frank Perroni
 * @brief This class represents the entire search space.
 * @details The search space in TH is represented as a Region with the "anchor" sub-region
 *          equals to the full search space.
 */

#ifndef SEARCHSPACE_H_
#define SEARCHSPACE_H_

#include "Region.h"

template <class P = double>
class SearchSpace : public Region<P> {
public:
	SearchSpace(map<Dimension<P>*, Partition<P>*> *partitions) : Region<P>(partitions) {}
	~SearchSpace(){}
};

#endif /* SEARCHSPACE_H_ */

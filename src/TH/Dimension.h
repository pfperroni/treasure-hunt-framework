/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file Dimension.h
 * @class Dimension
 * @author Peter Frank Perroni
 * @brief This class represents the boundaries of a dimension within the search space.
 */

#ifndef DIMENSION_H_
#define DIMENSION_H_

#include "Partition.h"

template <class P = double>
class Dimension : public Partition<P> {
public:
	Dimension(int ID, P startPoint, P endPoint) : Partition<P>(ID, startPoint, endPoint){}
	Dimension(Partition<P> *partition) : Partition<P>(partition){}
	~Dimension(){}
};

#endif /* DIMENSION_H_ */

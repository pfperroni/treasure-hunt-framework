/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For additional notifications, please check the file NOTICE.txt.
 *
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

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
 * @file Region.h
 * @class Region
 * @author Peter Frank Perroni
 * @brief This class represents a region within the search space.
 * @details A region in TH is represented by the set of all Partitions required to compose
 *          the "anchor" sub-region, besides all dimensions required to compose the entire
 *          search space.
 */

#ifndef REGION_H_
#define REGION_H_

#include "Dimension.h"
#include "Partition.h"
#include <map>
#include <cstdlib>
#include <sstream>
#include <iostream>

using namespace std;

template<class P = double>
class Region {
	map<Dimension<P>*, Partition<P>*> *partitions;
	map<int, Dimension<P>* > *dimensions;

	void setup(map<Dimension<P>*, Partition<P>*> *partitions) {
		if(partitions == NULL || partitions->size() == 0) {
			throw std::invalid_argument("The partitions that compose a region cannot be empty.");
		}
		dimensions = new map<int, Dimension<P>*>();
		this->partitions = new map<Dimension <P>*, Partition <P>*>();
		Dimension<P> *dim;
		Partition<P> *part;
		for(auto elem = partitions->begin(); elem != partitions->end(); ++elem) {
			dim = new Dimension<P>(elem->first);
			part = new Partition<P>(elem->second);
			this->dimensions->insert({dim->getID(), dim});
			this->partitions->insert({dim, part});
		}
	}

public:
	/**
	 * @brief This construction creates a Region by copying the contents of the map provided.
	 * @param partitions The source map.
	 */
	Region(map<Dimension<P>*, Partition<P>*> *partitions) { // @suppress("Class members should be properly initialized")
		setup(partitions);
	}

	/**
	 * @brief This construction creates a Region by copying the contents of the Region provided.
	 * @param partitions The source Region instance.
	 */
	Region(Region<P> *region) { // @suppress("Class members should be properly initialized")
		if(region == NULL) {
			throw std::invalid_argument("A region cannot be created based on an empty region.");
		}
		setup(region->partitions);
	}
	~Region(){
		for(auto elem = partitions->begin(); elem != partitions->end(); ++elem) {
			delete elem->first;
			delete elem->second;
		}
		delete partitions;
		delete dimensions;
	}

	/**
	 * @brief Operator that selects the dimension's Partition based on dimension's sequential index.
	 *
	 * The pointer to the actual object is returned, instead of a simple copy.
	 *
	 * @param i The index of the Dimension (index starts in zero).
	 * @return A pointer to the Partition selected.
	 */
	Partition<P>* operator [](int i) {
		Dimension<P> *dim = getOriginalDimension(i);
		if(dim != NULL) {
			typename map<Dimension <P>*, Partition<P>*>::iterator itP = partitions->find(dim);
			if(itP != partitions->end()) return itP->second;
		}
		return NULL;
	}

	/**
	 * @brief Obtains a pointer to the map containing the dimensions
	 *        that compose the entire search space.
	 *
	 * The pointer to the actual object is returned, instead of a simple copy.
	 *
	 * @return A pointer to the map containing the dimensions for entire search space.
	 */
	map<int, Dimension<P>*>* getOriginalDimensions(){
		return dimensions;
	}

	/**
	 * @brief Get the full search space's Dimension based on its sequential index.
	 *
	 * The pointer to the actual object is returned, instead of a simple copy.
	 *
	 * @param i The index of the Dimension (index starts in zero).
	 * @return A pointer to the Dimension selected.
	 */
	Dimension<P>* getOriginalDimension(int i){
		typename map<int, Dimension<P>*>::iterator itD = dimensions->find(i);
		if(itD != dimensions->end()) {
			return itD->second;
		}
		return NULL;
	}

	/**
	 * @brief Get the map of Partitions that compose the current "anchor" sub-region.
	 *
	 * The pointer to the actual object is returned, instead of a simple copy.
	 *
	 * @return The map that compose the current "anchor" sub-region.
	 */
	map<Dimension <P>*, Partition <P>*>* getPartitions(){
		return partitions;
	}

	/**
	 * @brief Get the number of dimensions of this region.
	 * @return The number of dimensions of this region.
	 */
	int getNDimensions() {
		return (dimensions!=NULL) ? dimensions->size() : 0;
	}

	/**
	 * @brief Debug the current Region instance.
	 */
	void print() {
		Dimension <P> *dim;
		Partition <P> *partition;
		std::stringstream ss;
		ss << "[ ";
		for(auto elem = partitions->begin(); elem != partitions->end(); ) {
			dim = elem->first;
			partition = elem->second;
			ss << "{ {" << dim->getID() << ", " << dim->getStartPoint() << ", " << dim->getEndPoint() << "}, "
					<< "{" << partition->getID() << ", " << partition->getStartPoint() << ", " << partition->getEndPoint() << "} }"
					<< (++elem != partitions->end() ? ", ": " ") << std::endl;
		}
		ss << "]";
		std::cout << ss.str().c_str() << std::endl;
	}
};

#endif /* REGION_H_ */

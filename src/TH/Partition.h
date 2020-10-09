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
 * @file Partition.h
 * @class Partition
 * @author Peter Frank Perroni
 * @brief This class represents an interval within dimension's boundaries.
 */

#ifndef PARTITION_H_
#define PARTITION_H_

#include <cstdlib>

template <class P = double>
class Partition {
	int ID;
	P startPoint;
	P endPoint;

	void setup(int ID, P startPoint, P endPoint) {
		this->ID = ID;
		this->startPoint = startPoint;
		this->endPoint = endPoint;
	}

public:
	/**
	 * @brief This constructor creates a Partition.
	 * @param ID The unique identifier for this partition.
	 * @param startPoint The lower bound of the interval.
	 * @param endPoint The upper bound of the interval.
	 */
	Partition(int ID, P startPoint, P endPoint) { // @suppress("Class members should be properly initialized")
		setup(ID, startPoint, endPoint);
	}

	/**
	 * @brief This constructor creates a Partition as a copy from another Partition.
	 * @param partition The source Partition instance.
	 */
	Partition(Partition *partition) { // @suppress("Class members should be properly initialized")
		if(partition == NULL) {
			throw std::invalid_argument("The partition cannot be empty.");
		}
		setup(partition->getID(), partition->getStartPoint(), partition->getEndPoint());
	}
	~Partition() {}

	/**
	 * @brief This operator checks only the partition IDs.
	 * @param p The Partition instance to compare.
	 * @return True if the current partition's ID is smaller than the parameter.
	 *         False otherwise.
	 */
	bool operator <(Partition p) { return this->ID < p.ID; }

	/**
	 * @brief This operator checks only the partition IDs.
	 * @param p The Partition instance to compare.
	 * @return True if the current partition's ID is larger than the parameter.
	 *         False otherwise.
	 */
	bool operator >(Partition p) { return this->ID > p.ID; }

	/**
	 * @brief Get the partition ID.
	 * @return The partition ID.
	 */
	int getID() { return ID; }

	/**
	 * @brief Get the lower bound for this partition.
	 * @return The lower bound for this partition.
	 */
	P getStartPoint() { return startPoint; }

	/**
	 * @brief Set the lower bound for this partition.
	 * @param startPoint The lower bound for this partition.
	 */
	void setStartPoint(P startPoint) {
		this->startPoint = startPoint;
	}

	/**
	 * @brief Get the upper bound for this partition.
	 * @return The upper bound for this partition.
	 */
	P getEndPoint() { return endPoint; }

	/**
	 * @brief Set the upper bound for this partition.
	 * @param endPoint The upper bound for this partition.
	 */
	void setEndPoint(P endPoint) {
		this->endPoint = endPoint;
	}
};

#endif /* PARTITION_H_ */

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
 * @file MpiTypeTraits.h
 * @class MpiTypeTraits
 * @author Peter Frank Perroni
 * @brief Structures for MPI.
 */

#ifndef TH_MPITYPETRAITS_H_
#define TH_MPITYPETRAITS_H_

#include <mpi.h>

template<class T>
struct MpiTypeTraits {
	static MPI_Datatype GetType() {
		return MPI_DATATYPE_NULL;
	}
};

template<>
struct MpiTypeTraits<int> {
	static MPI_Datatype GetType() {
		return MPI_INT;
	}
};

template<>
struct MpiTypeTraits<long> {
	static MPI_Datatype GetType() {
		return MPI_LONG;
	}
};

template<>
struct MpiTypeTraits<float> {
	static MPI_Datatype GetType() {
		return MPI_FLOAT;
	}
};

template<>
struct MpiTypeTraits<double> {
	static MPI_Datatype GetType() {
		return MPI_DOUBLE;
	}
};

template<>
struct MpiTypeTraits<char> {
	static MPI_Datatype GetType() {
		return MPI_CHAR;
	}
};

#endif /* TH_MPITYPETRAITS_H_ */

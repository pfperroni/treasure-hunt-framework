/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
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

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
 * @file macros.h
 * @author Peter Frank Perroni
 * @brief Macros and constants must defined in this file.
 */

#ifndef MACROS_H_
#define MACROS_H_

#include <stdio.h>
#include <sys/time.h>
#include <sstream>

enum{ MSG_STARTUP, MSG_CHILD2PARENT, MSG_PARENT2CHILD, MSG_FINALIZE };

#define DEBUG_NONE 0
#define DEBUG_BASIC 1
#define DEBUG_DETAILED 2

#define RANDBEHAVIOR_RANDOM 0
#define RANDBEHAVIOR_DETERMINISTIC 1	// Be aware that deterministic behavior also depends on external factors,
										// like the optimization algorithms and execution configurations (wall clock time, number of evaluations, etc).

#define COPY_ARR(orig, dest, sz) for(int _i_=0; _i_ < sz; (dest)[_i_] = (orig)[_i_], _i_++);

#ifdef DEBUG
#define GET_CURRTS(ts){ timeval _ts_; gettimeofday(&_ts_, 0); tm *_t_ = localtime(&_ts_.tv_sec); strftime(ts, 25, "%F %T ", _t_); }
#define PRINT_CURRTS(){ timeval _ts_; gettimeofday(&_ts_, 0); tm *_t_ = localtime(&_ts_.tv_sec); printf("%04d-%02d-%02d %02d:%02d:%02d.%03d ", _t_->tm_year+1900, _t_->tm_mon+1, _t_->tm_mday, _t_->tm_hour, _t_->tm_min, _t_->tm_sec, (int)_ts_.tv_usec/1000); }
char _buffer_[1024000];
#define DEBUG2FILE(ID, ...) { \
	char _fileName_[20]; \
	GET_CURRTS(_buffer_); \
	sprintf(&_buffer_[20], __VA_ARGS__); char _ch_ = '\n'; strncat(_buffer_, &_ch_, 1); \
	sprintf(_fileName_, "log%d.out", ID); \
	THUtil::writeToFile(_fileName_, _buffer_); \
}

#if DEBUG >= DEBUG_NONE
#define DEBUG_MANDATORY(...) { PRINT_CURRTS() printf(__VA_ARGS__); }
#define DEBUG_MANDATORY_IF(cond, ...) {if(cond) {PRINT_CURRTS() printf(__VA_ARGS__);}}
#define DEBUG2FILE_MANDATORY(ID, ...) DEBUG2FILE(ID, __VA_ARGS__)
#define DEBUG2FILE_MANDATORY_IF(ID, cond, ...) {if(cond) DEBUG2FILE(ID, __VA_ARGS__)}
#else
#define DEBUG_MANDATORY(...)
#define DEBUG_MANDATORY_IF(cond, ...)
#define DEBUG2FILE_MANDATORY(ID, ...)
#define DEBUG2FILE_MANDATORY_IF(ID, cond, ...)
#endif

#if DEBUG >= DEBUG_BASIC
#define DEBUG_INFO(...) { PRINT_CURRTS() printf(__VA_ARGS__); }
#define DEBUG_INFO_IF(cond, ...) {if(cond) {PRINT_CURRTS() printf(__VA_ARGS__);}}
#define DEBUG2FILE_INFO(ID, ...) DEBUG2FILE(ID, __VA_ARGS__)
#define DEBUG2FILE_INFO_IF(ID, cond, ...) {if(cond) DEBUG2FILE(ID, __VA_ARGS__)}
#else
#define DEBUG_INFO(...)
#define DEBUG_INFO_IF(cond, ...)
#define DEBUG2FILE_INFO(ID, ...)
#define DEBUG2FILE_INFO_IF(ID, cond, ...)
#endif

#if DEBUG == DEBUG_DETAILED
#define DEBUG_VECTOR_INT_IF(cond, id, label, table, size) \
	{ if(cond) { DEBUG_VECTOR_INT(id, label, table, size) } }
#define DEBUG_VECTOR_INT(id, label, table, size) \
	PRINT_CURRTS() \
	printf("TH[%i] %s: [", id, label); \
	for(int _i_=0; _i_ < size; _i_++) \
		printf(" %i ", table[_i_]); \
	printf("]\n");
#define DEBUG_VECTOR_DOUBLE_IF(cond, id, label, table, size) \
	{ if(cond) { DEBUG_VECTOR_DOUBLE(id, label, table, size) } }
#define DEBUG_VECTOR_DOUBLE(id, label, table, size) \
	PRINT_CURRTS() \
	printf("TH[%i] %s: [", id, label); \
	for(int _i_=0; _i_ < size; _i_++) \
		printf(" %f ", (table)[_i_]); \
	printf("]\n");
#define DEBUG_TABLE_INT_IF(cond, id, label, table, rows, cols) \
	{ if(cond) { DEBUG_TABLE_INT(id, label, table, rows, cols) } }
#define DEBUG_TABLE_INT(id, label, table, rows, cols) \
	PRINT_CURRTS() \
	printf("TH[%i] %s: {\n", id, label); \
	for(int _j_, _i_=0; _i_ < rows; _i_++){ \
		printf(" ["); \
		for(_j_=0; _j_ < cols; _j_++) \
			printf(" %i ", (table)[_i_][_j_]); \
		printf("]\n"); \
	} \
	printf("}\n");
#define DEBUG_TABLE_DOUBLE_IF(cond, id, label, table, rows, cols) \
	{ if(cond) { DEBUG_TABLE_DOUBLE(id, label, table, rows, cols) } }
#define DEBUG_TABLE_DOUBLE(id, label, table, rows, cols) \
	PRINT_CURRTS() \
	printf("TH[%i] %s: {\n", id, label); \
	for(int _j_, _i_=0; _i_ < rows; _i_++){ \
		printf(" ["); \
		for(_j_=0; _j_ < cols; _j_++) \
			printf(" %f ", table[_i_][_j_]); \
		printf("]\n"); \
	} \
	printf("}\n");
#define DEBUG_SOLUTION_DOUBLE(id, label, solution, rows, cols) \
	PRINT_CURRTS() \
	printf("TH[%i] %s: {\n", id, label); \
	for(int _j_, _i_=0; _i_ < rows; _i_++){ \
		printf(" ["); \
		for(_j_=0; _j_ < cols; _j_++) \
			printf(" %f ", (*(solution)[_i_])[_j_]->getFirstValue()); \
		printf("]\n"); \
	} \
	printf("}\n");
#define DEBUG_SOLUTION_INT(id, label, solution, rows, cols) \
	PRINT_CURRTS() \
	printf("TH[%i] %s: {\n", id, label); \
	for(int _j_, _i_=0; _i_ < rows; _i_++){ \
		printf(" ["); \
		for(_j_=0; _j_ < cols; _j_++) \
			printf(" %i ", (*(solution)[_i_])[_j_]->getFirstValue()); \
		printf("]\n"); \
	} \
	printf("}\n");
#define DEBUG_REGION(label, region) { \
	PRINT_CURRTS() \
	std::stringstream ss; \
	ss << label << ": {" << std::endl; \
	for(int _i_=0, _ndim_=region->getNDimensions(); _i_ < _ndim_; _i_++){ \
		ss << " [ " << (*region)[_i_]->getStartPoint() << " " << (*region)[_i_]->getEndPoint() << " ] "; \
	} \
	std::cout << ss.str() << std::endl << "}" << std::endl; \
}
#define DEBUG2FILE_REGION(ID, label, region) { \
	std::stringstream ss; \
	ss << label << ": {" << std::endl; \
	for(int _i_=0, _ndim_=region->getNDimensions(); _i_ < _ndim_; _i_++){ \
		ss << " [ " << (*region)[_i_]->getStartPoint() << " " << (*region)[_i_]->getEndPoint() << " ] "; \
	} \
	ss << std::endl << "}" << std::endl; \
	DEBUG2FILE(ID, "%s", ss.str().c_str()); \
}
#define DEBUG_INT(label, value) printf("%s: %i\n", label, s->gFitness);
#define DEBUG_DOUBLE(label, value) printf("%s: %f\n", label, s->gFitness);
#define DEBUG_TEXT(...) { PRINT_CURRTS() printf(__VA_ARGS__); }
#define DEBUG_TEXT_IF(cond, ...) {if(cond) { PRINT_CURRTS() printf(__VA_ARGS__); } }
#define DEBUG2FILE_TEXT(ID, ...) DEBUG2FILE(ID, __VA_ARGS__)
#define DEBUG2FILE_TEXT_IF(ID, cond, ...) {if(cond) DEBUG2FILE(ID, __VA_ARGS__)}
#else
#define DEBUG_VECTOR_INT_IF(cond, id, label, table, size)
#define DEBUG_VECTOR_INT(id, label, table, size)
#define DEBUG_VECTOR_DOUBLE_IF(cond, id, label, table, size)
#define DEBUG_VECTOR_DOUBLE(id, label, table, size)
#define DEBUG_TABLE_INT_IF(cond, id, label, table, rows, cols)
#define DEBUG_TABLE_INT(id, label, table, rows, cols)
#define DEBUG_TABLE_DOUBLE_IF(cond, id, label, table, rows, cols)
#define DEBUG_TABLE_DOUBLE(id, label, table, rows, cols)
#define DEBUG_SOLUTION_DOUBLE(id, label, solution, rows, cols)
#define DEBUG_SOLUTION_INT(id, label, solution, rows, cols)
#define DEBUG_REGION(label, region)
#define DEBUG2FILE_REGION(ID, label, region)
#define DEBUG_INT(label, value)
#define DEBUG_DOUBLE(label, value)
#define DEBUG_TEXT(...)
#define DEBUG_TEXT_IF(cond, ...)
#define DEBUG2FILE_TEXT(ID, ...)
#define DEBUG2FILE_TEXT_IF(ID, cond, ...)
#endif

#else
#define DEBUG_MANDATORY(...)
#define DEBUG_MANDATORY_IF(cond, ...)
#define DEBUG_VECTOR_INT_IF(cond, id, label, table, size)
#define DEBUG_VECTOR_INT(id, label, table, size)
#define DEBUG_VECTOR_DOUBLE_IF(cond, id, label, table, size)
#define DEBUG_VECTOR_DOUBLE(id, label, table, size)
#define DEBUG_TABLE_INT_IF(cond, id, label, table, rows, cols)
#define DEBUG_TABLE_INT(id, label, table, rows, cols)
#define DEBUG_TABLE_DOUBLE_IF(cond, id, label, table, rows, cols)
#define DEBUG_TABLE_DOUBLE(id, label, table, rows, cols)
#define DEBUG_SOLUTION_DOUBLE(id, label, solution, rows, cols)
#define DEBUG_SOLUTION_INT(id, label, solution, rows, cols)
#define DEBUG_REGION(label, region)
#define DEBUG2FILE_REGION(ID, label, region)
#define DEBUG_INT(label, value)
#define DEBUG_DOUBLE(label, value)
#define DEBUG_INFO(...)
#define DEBUG_INFO_IF(cond, ...)
#define DEBUG_TEXT(...)
#define DEBUG_TEXT_IF(cond, ...)
#define DEBUG2FILE_MANDATORY(ID, ...)
#define DEBUG2FILE_MANDATORY_IF(ID, cond, ...)
#define DEBUG2FILE_INFO(ID, ...)
#define DEBUG2FILE_INFO_IF(ID, cond, ...)
#define DEBUG2FILE_TEXT(ID, ...)
#define DEBUG2FILE_TEXT_IF(ID, cond, ...)
#endif

#endif /* MACROS_H_ */

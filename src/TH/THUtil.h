/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file THUtil.h
 * @class THUtil
 * @author Peter Frank Perroni
 * @brief Static utility methods.
 */

#ifndef THUTIL_H_
#define THUTIL_H_

#include <string.h>
#include <random>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sys/stat.h>
#include <libgen.h>
#include "macros.h"
#include "config.h"

class THUtil{
public:

	/**
	 * @brief Get a random seed.
	 *
	 * For non unix-like environments, a sequential seed will be returned.
	 *
	 * @return A random seed.
	 */
	static unsigned int getRandomSeed(){
		static unsigned int randomSeq = 0;
		unsigned int randVal;
		bool random = true;

#ifdef RANDBEHAVIOR
#if RANDBEHAVIOR == RANDBEHAVIOR_DETERMINISTIC
		random = false;
#endif
#endif

		int sizeRead = 0;
		if(random) { // Cannot be inside an ifdef due to user configuration.
			FILE *f = fopen("/dev/urandom", "r");
			sizeRead = fread(&randVal, sizeof(randVal), 1, f);
			fclose(f);
		}

		if(!random || sizeRead == 0){
			randVal = ++randomSeq;
		}

		return randVal;
	}

	static void truncateFile(const char* fileName) {
		std::ofstream file;
		file.open(fileName, ios::trunc);
		if(file.is_open()) {
			file.close();
		}
	}

	static void writeToFile(const char* fileName, const char* content) {
		std::ofstream file;
		file.open(fileName, ios::out | ios::app);
		if(file.is_open()) {
			file << content;
			file.close();
		}
	}

	static void writeToFile(const char* fileName, vector<string> &content) {
		std::ofstream file;
		file.open(fileName, ios::out | ios::app);
		if(file.is_open()) {
			for(string s : content) {
				file << s << endl;
			}
			file.close();
		}
	}

	static void readFile(const char* fileName, vector<string> &data) {
		std::ifstream file;
		file.open(fileName, ios::in);
		if(file.is_open()) {
			stringstream ss;
			string line;
			ss << file.rdbuf();
			while(getline(ss, line, '\n')) {
				data.push_back(string(line));
			}
			file.close();
		}
	}

	static inline bool existsFile(const char* fileName) {
		FILE *file = fopen(fileName, "r");
		if(file) {
			fclose(file);
			return true;
		}
		return false;
	}

	bool isFolder(const char *path) {
		if(path == NULL) return false;
	    struct stat stats;
	    stat(path, &stats);
	    if(S_ISDIR(stats.st_mode)) return true;
	    return false;
	}

	/**
	 * Return the directory path portion of a full path.
	 * The char pointer must be freed up through the free() function.
	 */
	char* getPath(const char* path){
		if(path == NULL) return NULL;
		char *buffer1 = strdup(path); // Temporary buffer.
		char *buffer2 = strdup(path); // Final buffer.
		char* ptr = dirname(buffer1); // Pointer to the actual path inside the temporary buffer.
		strcpy(buffer2, ptr); // Copy the actual path to the final buffer.
		free(buffer1); // Delete the temporary buffer
		return buffer2; // Return the final buffer, that must be freed up later.
	}

	static inline double randUniformDouble(unsigned int &seed, double a, double b) { return (a==b) ? a : (a+(((double)rand_r(&seed))/RAND_MAX)*(b-a)); }
	static inline int randUniformInt(unsigned int &seed, int a, int b) { return (int)randUniformDouble(seed, (double)a, b+0.99); }

	static inline double randNormalDouble(unsigned int &seed, double a, double b) {
		mt19937 gen(seed);
		normal_distribution<double> normal(0.5, 1);
		return (a==b) ? a : a+(min(max(normal(gen), 0.0), 1.1)*(b-a));
	}
	static inline int randNormalInt(unsigned int &seed, int a, int b) { return (int)randNormalDouble(seed, (double)a, b+0.99); }

};

#endif /* THUTIL_H_ */

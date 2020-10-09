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
 * @file TH_example.h
 * @author Peter Frank Perroni
 * @brief Example of use of Treasure Hunt Framework with
 *        1 single TH instance and 1 single search algorithm.
 */

#include <iostream>
#include <map>

#include "config.h"

#include "../TH/THBuilder.h"
#include "../TH/Solution.h"
#include "../TH/SingleSearchAlgorithmSelectionPolicy.h"
#include "../RosenbrockFitnessPolicy.h"
#include "../PSO.h"

template<class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
void printSolution(Solution<P, pSize, F, fSize, V, vSize> *solution) {
	int sz = solution->getNDimensions() * pSize;
	P positions[sz];
	solution->getPositions(positions);
	std::cout << "{ ";
	for(int i=0; i < sz; i++){
		std::cout << positions[i] << " ";
	}
	std::cout << "}";
}

template<class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
void runTH(int argc, char *argv[]) {
	int i;

	// Create the search space boundaries.
	map<Dimension<P>*, Partition<P>*> *partitions = new map<Dimension<P>*, Partition<P>*>();
	Dimension<P>* dim;
	int n = 1000;
	for(i=0; i < n; i++){
		dim = new Dimension<P>(i, -20, 20); // Dimension limits.
		partitions->insert({dim, dim}); // Partition equals dimension boundaries for full search space.
	}

	// Mount the TH tree topology.
	THTree* thTree = new THTree(1);
	thTree->addRootNode(0);

	// Set the configuration required to build the TH instance.
	THBuilder<P, pSize, F, fSize, V, vSize> *thBuilder = new THBuilder<P, pSize, F, fSize, V, vSize>();
	thBuilder->setMpiComm(argc, argv)
			->setTHTree(thTree)
			->setSearchSpace(new SearchSpace<P>(partitions))
			->setFitnessPolicy(new RosenbrockFitnessPolicy())
			->setSearchAlgorithmSelectionPolicy(new SingleSearchAlgorithmSelectionPolicy<P, pSize, F, fSize, V, vSize>())
			->addSearchAlgorithm(new PSO<P, pSize, F, fSize, V, vSize>(0.9, 0.7, 0.7, 12))
			->setBestListSize(2)
			->setMaxTimeSeconds(100);

	// Build the TH instance.
	TH<P, pSize, F, fSize, V, vSize> *th = thBuilder->build();

	// Execute the TH instance.
	th->run();

	// Obtain the final result.
	int ID = th->getID();
	std::cout << "[" << ID << "] Best Result: Num.Evals = " << th->getNEvals()
				<< ", Fitness = " << th->getBestSolution()->getFitness()->getFirstValue() << std::endl;
	if(ID == thTree->getRootNode()->getID()) {
		std::cout << "Overal Best Solution : ";
		printSolution(th->getBestSolution());
		std::cout << std::endl;
		BestList<P, pSize, F, fSize, V, vSize>* bestList = th->getBestList();
		if(bestList != NULL) {
			for(i=0; i < bestList->getListSize(); i++){
				std::cout << "BestList[" << i << "]: ";
				printSolution((*bestList)[i]);
				std::cout << std::endl;
			}
		}
	}

	for(auto elem = partitions->begin(); elem != partitions->end(); ++elem) {
		delete (Dimension<>*)elem->first;
	}
	delete partitions;
	delete th; // Do NOT delete the builder, since it will be deleted by TH instance.
}

int main(int argc, char *argv[]) {
	runTH<>(argc, argv);
	return 0;
}



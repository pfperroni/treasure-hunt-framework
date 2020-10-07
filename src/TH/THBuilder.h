/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file THBuilder.h
 * @class THBuilder
 * @author Peter Frank Perroni
 * @brief Treasure Hunt Framework Builder.
 * @details Before a TH can be instanced, its configurations must be
 *          build by this convenience builder.
 * @note For Treasure Hunt Framework, please refer (and cite) to
 *       the following:\n
 *       <em>PERRONI, Peter Frank (2019). Treasure hunt: a framework for cooperative,
 *       distributed parallel optimization (Doctoral thesis, UFPR).
 *       Institutional repository at UFPR: https://acervodigital.ufpr.br/handle/1884/62517</em>
 */

#ifndef THBUILDER_H_
#define THBUILDER_H_

#include "config.h"
#include "macros.h"

#include "TH.h"

#include "CSMOn.h"
#include "BetaRelocationStrategyData.h"
#include "HillClimbing.h"
#include "RandomBestListSelectionPolicy.h"

#include "THTree.h"
#include "Solution.h"
#include "BestList.h"
#include "BestListSelectionPolicy.h"
#include "BestListUpdatePolicy.h"
#include "ConvergentBestListUpdatePolicy.h"
#include "IterationData.h"
#include "FitnessPolicy.h"
#include "Region.h"
#include "RegionSelectionPolicy.h"
#include "GroupRegionSelectionPolicy.h"
#include "RelocationStrategyData.h"
#include "Search.h"
#include "SearchAlgorithmSelectionPolicy.h"
#include "SearchScore.h"
#include "SearchSpace.h"
#include "RoundRobinSearchAlgorithmSelectionPolicy.h"
#include "BetaRelocationStrategyPolicy.h"
#include "ConvergenceControlPolicy.h"
#include "RelocationStrategyPolicy.h"
#include "THUtil.h"
#include "MpiTypeTraits.h"

#include <stddef.h>
#include <stdexcept>
#include <vector>

#include <execinfo.h>
#include <signal.h>


/**
 * A handler to receive the exceptions during running time.
 */
void signalActionHandler(int signal, siginfo_t* info, void* ptr) {
	void *messages[256];
	size_t size = backtrace(messages, 256);
	fprintf(stderr, "System Error, signal received: %d\n", signal);
	backtrace_symbols_fd(messages, size, STDERR_FILENO);
	exit(1);
}

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class THBuilder {

	THTree *thTree;
	RegionSelectionPolicy<P> *regionSelectionPolicy;
	SearchSpace<P> *searchSpace;
	Region<P> *subRegion;
	FitnessPolicy<P, pSize, F, fSize, V, vSize> *fitnessPolicy;
	ConvergenceControlPolicy<P, pSize, F, fSize, V, vSize> *convergenceControl;
	BestListUpdatePolicy<P, pSize, F, fSize, V, vSize> *bestListUpdatePolicy;
	BestListSelectionPolicy<P, pSize, F, fSize, V, vSize> *bestListSelectionPolicy;
	RelocationStrategyPolicy<P, pSize, F, fSize, V, vSize> *relocationStrategyPolicy;
	RelocationStrategyData<P, pSize, F, fSize, V, vSize> *relocationStrategyData;
	SearchAlgorithmSelectionPolicy<P, pSize, F, fSize, V, vSize> *searchAlgorithmSelectionPolicy;

	Search<P, pSize, F, fSize, V, vSize> *localSearchAlgorithm;
	vector<SearchScore<P, pSize, F, fSize, V, vSize>*> *searchAlgorithms;
	BestList<P, pSize, F, fSize, V, vSize> *bestList;
	Solution<P, pSize, F, fSize, V, vSize> *generalBest;
	Solution<P, pSize, F, fSize, V, vSize> *bias;
	Solution<P, pSize, F, fSize, V, vSize> **startupSolutions;
	TH<P, pSize, F, fSize, V, vSize> *th;

	bool built;
	int ID;
	MPI_Comm cartGrid;
	long maxNumberEvaluations;
	long maxTimeSeconds;
	long long maxIterations;
	int bestListSize;
	long long nEvals;
	long double elapsedSeconds;
	int nStartupSolutions;

	struct sigaction newSignalAction, oldSignalAction;

	void setBestList(BestList<P, pSize, F, fSize, V, vSize> *bestList) {
		this->bestList = bestList;
	}

	void setGeneralBest(Solution<P, pSize, F, fSize, V, vSize> *generalBest) {
		this->generalBest = generalBest;
	}

	void incrementEvals(int incr){
		nEvals += incr;
	}

	void setElapsedSeconds(long double elapsedSeconds) {
		this->elapsedSeconds = elapsedSeconds;
	}

	long double getElapsedSeconds() {
		return elapsedSeconds;
	}

	BestList<P, pSize, F, fSize, V, vSize>* getBestList() {
		return bestList;
	}

	Solution<P, pSize, F, fSize, V, vSize>* getGeneralBest() {
		return generalBest;
	}

	Region<P>* getSubRegion() {
		return subRegion;
	}

	void setSubRegion(Region<P>* subRegion) {
		this->subRegion = subRegion;
	}

public:
	/**
	 * @brief Constructor to create a THBuilder instance.
	 *
	 * ATTENTION: this builder can be deleted as long as it has not been built yet.
	 * Once the method {@link build()} has been called, do Not delete the TH builder
	 * because it will be deleted by the TH instance!
	 */
	THBuilder(){
		searchAlgorithms = new vector<SearchScore<P, pSize, F, fSize, V, vSize>*>();
		localSearchAlgorithm = NULL;
		bestList = NULL;
		generalBest = bias = NULL;
		startupSolutions = NULL;

		th = NULL;
		thTree = NULL;
		regionSelectionPolicy = NULL;
		fitnessPolicy = NULL;
		convergenceControl = NULL;
		bestListUpdatePolicy = NULL;
		bestListSelectionPolicy = NULL;
		relocationStrategyPolicy = NULL;
		relocationStrategyData = NULL;
		searchSpace = NULL;
		subRegion = NULL;
		searchAlgorithmSelectionPolicy = NULL;

		built = false;
		cartGrid = NULL;
		ID = 0;
		maxNumberEvaluations = 0;
		maxTimeSeconds = 0;
		maxIterations = 0;
		nEvals = 0;
		elapsedSeconds = 0;
		bestListSize = 1;
		nStartupSolutions = 0;

		newSignalAction.sa_flags = SA_SIGINFO;
		newSignalAction.sa_sigaction = signalActionHandler;
		sigaction(SIGABRT, &newSignalAction, &oldSignalAction);
	}
	~THBuilder(){
		if(fitnessPolicy != NULL) delete fitnessPolicy;
		if(regionSelectionPolicy != NULL) delete regionSelectionPolicy;
		if(relocationStrategyData != NULL) delete relocationStrategyData;
		if(convergenceControl != NULL) delete convergenceControl;
		if(bestListUpdatePolicy != NULL) delete bestListUpdatePolicy;
		if(bestListSelectionPolicy != NULL) delete bestListSelectionPolicy;
		if(bias != NULL) delete bias;
		if(relocationStrategyPolicy != NULL) delete relocationStrategyPolicy;
		if(searchSpace != NULL) delete searchSpace;
		if(localSearchAlgorithm != NULL) delete localSearchAlgorithm;
		if(searchAlgorithmSelectionPolicy != NULL) delete searchAlgorithmSelectionPolicy;
		for (int i=0; i < (int)searchAlgorithms->size(); i++) delete searchAlgorithms->at(i);
		delete searchAlgorithms;
		for (int i=0; i < nStartupSolutions; i++) delete startupSolutions[i];
		delete startupSolutions;
		if(built) MPI_Finalize();
		if(thTree != NULL) delete thTree;

		DEBUG_TEXT("Objects deallocated from TH[%i]\n", ID);
		DEBUG2FILE_TEXT(ID, "Objects deallocated from TH[%i]\n", ID);
	}

	/**
	 * @brief Build the TH implementation based on the configuration.
	 *
	 * ATTENTION: do Not delete the TH builder because it will be deleted
	 * by the TH instance!
	 *
	 * @return The TH implementation ready to run.
	 */
	TH<P, pSize, F, fSize, V, vSize>* build(){
		if(built){
			throw std::invalid_argument("This builder has already constructed an implementation.");
		}

		built = true;
		DEBUG_TEXT("Constructing TH[%i]...\n", ID);
		DEBUG2FILE_TEXT(ID, "Constructing TH[%i]...\n", ID);

		th = new THImpl(this);

		return th;
	}

	/**
	 * @brief Start the MPI environment.
	 * @param argc The operating system argc.
	 * @param argv The operating system argv.
	 * @return A pointer to this builder.
	 */
	THBuilder<P, pSize, F, fSize, V, vSize>* setMpiComm(int argc, char *argv[]){
		MPI_Init(&argc, &argv);
		setMpiComm(MPI_COMM_WORLD);
		return this;
	}

	/**
	 * @brief Start the MPI environment.
	 * @param mpiComm The MPI communicator.
	 * @return A pointer to this builder.
	 */
	THBuilder<P, pSize, F, fSize, V, vSize>* setMpiComm(MPI_Comm mpiComm){
		if(mpiComm == NULL){
			throw std::invalid_argument("MPI communication was not started.");
		}
		int id, nProcs;
		int dims[1] = {0}, periodic[1] = {0};

		MPI_Comm_rank(mpiComm, &id);
		MPI_Comm_size(mpiComm, &nProcs);

		MPI_Dims_create(nProcs, 1, dims);
		MPI_Cart_create(mpiComm, 1, dims, periodic, 0, &cartGrid);
		MPI_Comm_rank(cartGrid, &ID);

		return this;
	}

	TH<P, pSize, F, fSize, V, vSize>* getTH() {
		return th;
	}

	THTree* getTHTree() {
		return thTree;
	}

	/**
	 * @brief Set the THTree topology.
	 *
	 * If a THTree has already been set, it will be deleted
	 * before setting the new instance.
	 *
	 * @param thTree The TH tree topology.
	 * @return A pointer to this builder.
	 */
	THBuilder<P, pSize, F, fSize, V, vSize>* setTHTree(THTree *thTree){
		if(thTree != NULL) {
			if(this->thTree != NULL) delete this->thTree;
			this->thTree = thTree;
		}
		return this;
	}

	/**
	 * @brief Get the BestListSelectionPolicy configured.
	 *
	 * If no best-list selection policy is configured, the
	 * RandomBestListSelectionPolicy will be set automatically.
	 *
	 * @return The BestListSelectionPolicy configured.
	 */
	BestListSelectionPolicy<P, pSize, F, fSize, V, vSize>* getBestListSelectionPolicy() {
		if(bestListSelectionPolicy == NULL) {
			bestListSelectionPolicy = new RandomBestListSelectionPolicy<P, pSize, F, fSize, V, vSize>();
		}
		return bestListSelectionPolicy;
	}

	/**
	 * @brief Set the BestListSelectionPolicy.
	 *
	 * If a BestListSelectionPolicy has already been set, it will be deleted
	 * before setting the new instance.
	 *
	 * @param bestListSelectionPolicy The best-list selection policy to be used.
	 * @return A pointer to this builder.
	 */
	THBuilder<P, pSize, F, fSize, V, vSize>* setBestListSelectionPolicy(
			BestListSelectionPolicy<P, pSize, F, fSize, V, vSize> *bestListSelectionPolicy) {
		if(bestListSelectionPolicy != NULL) {
			if(this->bestListSelectionPolicy != NULL) delete this->bestListSelectionPolicy;
			this->bestListSelectionPolicy = bestListSelectionPolicy;
		}
		return this;
	}

	/**
	 * @brief Get the BestListUpdatePolicy configured.
	 *
	 * If no best-list update policy is configured, the
	 * ConvergentBestListUpdatePolicy will be set automatically.
	 *
	 * @return The BestListUpdatePolicy configured.
	 */
	BestListUpdatePolicy<P, pSize, F, fSize, V, vSize>* getBestListUpdatePolicy() {
		if(bestListUpdatePolicy == NULL) {
			bestListUpdatePolicy = new ConvergentBestListUpdatePolicy<P, pSize, F, fSize, V, vSize>();
		}
		return bestListUpdatePolicy;
	}

	/**
	 * @brief Set the BestListUpdatePolicy.
	 *
	 * If a BestListUpdatePolicy has already been set, it will be deleted
	 * before setting the new instance.
	 *
	 * @param bestListUpdatePolicy The best-list update policy to be used.
	 * @return A pointer to this builder.
	 */
	THBuilder<P, pSize, F, fSize, V, vSize>* setBestListUpdatePolicy(
			BestListUpdatePolicy<P, pSize, F, fSize, V, vSize> *bestListUpdatePolicy) {
		if(bestListUpdatePolicy != NULL) {
			if(this->bestListUpdatePolicy != NULL) delete this->bestListUpdatePolicy;
			this->bestListUpdatePolicy = bestListUpdatePolicy;
		}
		return this;
	}

	Solution<P, pSize, F, fSize, V, vSize>* getBias() {
		return bias;
	}

	/**
	 * @brief Define a bias for the search.
	 *
	 * Despite the fact that academic optimization problems usually do not involve bias,
	 * a bias can be really useful on real-world problems.
	 *
	 * @param bias A bias for the search.
	 * @return A pointer to this builder.
	 */
	THBuilder<P, pSize, F, fSize, V, vSize>* setBias(Solution<P, pSize, F, fSize, V, vSize> *bias) {
		if(bias != NULL) {
			if(this->bias != NULL) delete this->bias;
			this->bias = bias;
		}
		return this;
	}

	/**
	 * @brief Get the ConvergenceControlPolicy configured.
	 *
	 * If no convergence control policy is configured, the
	 * CSMOn will be set automatically.
	 *
	 * @return The ConvergenceControlPolicy configured.
	 */
	ConvergenceControlPolicy<P, pSize, F, fSize, V, vSize>* getConvergenceControlPolicy() {
		if(convergenceControl == NULL) {
			convergenceControl = new CSMOn<P, pSize, F, fSize, V, vSize>(3e3, 0.2, getFitnessPolicy()->getMinEstimatedFitnessValue());
		}
		return convergenceControl;
	}

	/**
	 * @brief Set the ConvergenceControlPolicy.
	 *
	 * If a ConvergenceControlPolicy has already been set, it will be deleted
	 * before setting the new instance.
	 *
	 * @param convergenceControl The convergence control policy to be used.
	 * @return A pointer to this builder.
	 */
	THBuilder<P, pSize, F, fSize, V, vSize>* setConvergenceControlPolicy(
			ConvergenceControlPolicy<P, pSize, F, fSize, V, vSize> *convergenceControl) {
		if(convergenceControl != NULL) {
			if(this->convergenceControl != NULL) delete this->convergenceControl;
			this->convergenceControl = convergenceControl;
		}
		return this;
	}

	/**
	 * @brief Get the RelocationStrategyData configured.
	 *
	 * If no relocation strategy storage is configured, the
	 * BetaRelocationStrategyData will be set automatically
	 * (given that BetaRelocationStrategyPolicy is the default).
	 *
	 * @return The RelocationStrategyData configured.
	 */
	RelocationStrategyData<P, pSize, F, fSize, V, vSize>* getRelocationStrategyData() {
		if(relocationStrategyData == NULL){
			relocationStrategyData = new BetaRelocationStrategyData<P, pSize, F, fSize, V, vSize>(0.99, 1, 1, 1);
		}
		return relocationStrategyData;
	}

	/**
	 * @brief Set the RelocationStrategyData.
	 *
	 * If a RelocationStrategyData has already been set, it will be deleted
	 * before setting the new instance.
	 *
	 * @param relocationStrategyData The relocation strategy storage to be used.
	 * @return A pointer to this builder.
	 */
	THBuilder<P, pSize, F, fSize, V, vSize>* setRelocationStrategyData(
			RelocationStrategyData<P, pSize, F, fSize, V, vSize> *relocationStrategyData) {
		if(relocationStrategyData != NULL) {
			if(this->relocationStrategyData != NULL) delete this->relocationStrategyData;
			this->relocationStrategyData = relocationStrategyData;
		}
		return this;
	}

	/**
	 * @brief Get the RelocationStrategyPolicy configured.
	 *
	 * If no relocation strategy policy is configured, the
	 * BetaRelocationStrategyPolicy will be set automatically.
	 *
	 * @return The RelocationStrategyPolicy configured.
	 */
	RelocationStrategyPolicy<P, pSize, F, fSize, V, vSize>* getRelocationStrategyPolicy() {
		if(relocationStrategyPolicy == NULL){
			relocationStrategyPolicy = new BetaRelocationStrategyPolicy<P, pSize, F, fSize, V, vSize>();
		}
		return relocationStrategyPolicy;
	}

	/**
	 * @brief Set the RelocationStrategyPolicy.
	 *
	 * If a RelocationStrategyPolicy has already been set, it will be deleted
	 * before setting the new instance.
	 *
	 * @param relocationStrategyUpdatePolicy The relocation strategy policy to be used.
	 * @return A pointer to this builder.
	 */
	THBuilder<P, pSize, F, fSize, V, vSize>* setRelocationStrategyPolicy(
			RelocationStrategyPolicy<P, pSize, F, fSize, V, vSize> *relocationStrategyUpdatePolicy) {
		if(relocationStrategyUpdatePolicy != NULL) {
			if(this->relocationStrategyPolicy != NULL) delete this->relocationStrategyPolicy;
			this->relocationStrategyPolicy = relocationStrategyUpdatePolicy;
		}
		return this;
	}

	/**
	 * @brief Get the local search algorithm configured.
	 *
	 * If no local search algorithm is configured, the
	 * HillClimbing will be set automatically.
	 *
	 * A local search algorithm is used on TH to refine results
	 * received from children TH instances.
	 *
	 * @return The local search algorithm configured.
	 */
	Search<P, pSize, F, fSize, V, vSize>* getLocalSearchAlgorithm() {
		if(localSearchAlgorithm == NULL){
			localSearchAlgorithm = new HillClimbing<P, pSize, F, fSize, V, vSize>(0.05, 1e-3, 1);
		}
		return localSearchAlgorithm;
	}

	/**
	 * @brief Set the local search algorithm.
	 *
	 * If a local search algorithm has already been set, it will be deleted
	 * before setting the new instance.
	 *
	 * @param localSearchAlgorithm The local search algorithm to be used.
	 * @return A pointer to this builder.
	 */
	THBuilder<P, pSize, F, fSize, V, vSize>* setLocalSearchAlgorithm(
				Search<P, pSize, F, fSize, V, vSize> *localSearchAlgorithm) {
		if(localSearchAlgorithm != NULL) {
			if(this->localSearchAlgorithm != NULL) delete this->localSearchAlgorithm;
			this->localSearchAlgorithm = localSearchAlgorithm;
		}
		return this;
	}

	/**
	 * @brief Get the search algorithms configured.
	 *
	 * No search algorithm is configured automatically.
	 *
	 * @return The search algorithms configured.
	 */
	vector<SearchScore<P, pSize, F, fSize, V, vSize>*>* getSearchAlgorithms(){
		return searchAlgorithms;
	}

	/**
	 * @brief Add a search algorithm to the TH instance.
	 * @param searchAlgorithm The search algorithm to add.
	 * @param weight The weight of this search algorithm for the scoring metrics.
	 * @return A pointer to this builder.
	 */
	THBuilder<P, pSize, F, fSize, V, vSize>* addSearchAlgorithm(
			Search<P, pSize, F, fSize, V, vSize> *searchAlgorithm, double weight = 1){
		if(searchAlgorithm != NULL) {
			searchAlgorithms->push_back(new SearchScore<P, pSize, F, fSize, V, vSize>(searchAlgorithm, weight));
		}
		return this;
	}

	/**
	 * @brief The largest 'preferred population size' from all search algorithms configured.
	 *
	 * TH instance's population will be created with this number of individuals,
	 * so that all search algorithms can run without further adaptation.
	 *
	 * @return The largest 'preferred population size'.
	 */
	int getMaxPopulationSize() {
		int populationSize = 0;
		for(SearchScore<P, pSize, F, fSize, V, vSize> *ss : *searchAlgorithms){
			Search<P, pSize, F, fSize, V, vSize> *search = ss->getSearchAlgorithm();
			if(search != NULL && search->getPreferredPopulationSize() > populationSize){
				populationSize = search->getPreferredPopulationSize();
			}
		}
		return populationSize;
	}

	/**
	 * @brief Get the SearchAlgorithmSelectionPolicy configured.
	 *
	 * If no search algorithm selection policy is configured, the
	 * RoundRobinSearchAlgorithmSelectionPolicy will be set automatically.
	 *
	 * @return The SearchAlgorithmSelectionPolicy configured.
	 */
	SearchAlgorithmSelectionPolicy<P, pSize, F, fSize, V, vSize>* getSearchAlgorithmSelectionPolicy() {
		if(searchAlgorithmSelectionPolicy == NULL){
			searchAlgorithmSelectionPolicy = new RoundRobinSearchAlgorithmSelectionPolicy<P, pSize, F, fSize, V, vSize>();
		}
		return searchAlgorithmSelectionPolicy;
	}

	/**
	 * @brief Set the SearchAlgorithmSelectionPolicy.
	 *
	 * If a SearchAlgorithmSelectionPolicy has already been set, it will be deleted
	 * before setting the new instance.
	 *
	 * @param searchAlgorithmSelectionPolicy The search algorithm selection policy to be used.
	 * @return A pointer to this builder.
	 */
	THBuilder<P, pSize, F, fSize, V, vSize>* setSearchAlgorithmSelectionPolicy(
			SearchAlgorithmSelectionPolicy<P, pSize, F, fSize, V, vSize> *searchAlgorithmSelectionPolicy) {
		if(searchAlgorithmSelectionPolicy != NULL) {
			if(this->searchAlgorithmSelectionPolicy != NULL) delete this->searchAlgorithmSelectionPolicy;
			this->searchAlgorithmSelectionPolicy = searchAlgorithmSelectionPolicy;
		}
		return this;
	}

	/**
	 * @brief Get the FitnessPolicy configured.
	 *
	 * No fitness policy is configured automatically.
	 *
	 * @return The fitness policy configured.
	 */
	FitnessPolicy<P, pSize, F, fSize, V, vSize>* getFitnessPolicy() {
		return fitnessPolicy;
	}

	/**
	 * @brief Set the FitnessPolicy.
	 *
	 * If a FitnessPolicy has already been set, it will be deleted
	 * before setting the new instance.
	 *
	 * @param fitnessPolicy The fitness policy to be used.
	 * @return A pointer to this builder.
	 */
	THBuilder<P, pSize, F, fSize, V, vSize>* setFitnessPolicy(
			FitnessPolicy<P, pSize, F, fSize, V, vSize> *fitnessPolicy) {
		if(fitnessPolicy != NULL) {
			if(this->fitnessPolicy != NULL) delete this->fitnessPolicy;
			this->fitnessPolicy = fitnessPolicy;
		}
		return this;
	}

	/**
	 * @brief Get the RegionSelectionPolicy configured.
	 *
	 * If no region selection policy is configured, the
	 * GroupRegionSelectionPolicy will be set automatically
	 * without any search space partitioning.
	 *
	 * @return The RegionSelectionPolicy configured.
	 */
	RegionSelectionPolicy<P, pSize, F, fSize, V, vSize>* getRegionSelectionPolicy() {
		if(regionSelectionPolicy == NULL){
			regionSelectionPolicy = new GroupRegionSelectionPolicy<P, pSize, F, fSize, V, vSize>(1, 1);
		}
		return regionSelectionPolicy;
	}

	/**
	 * @brief Set the RegionSelectionPolicy.
	 *
	 * If a RegionSelectionPolicy has already been set, it will be deleted
	 * before setting the new instance.
	 *
	 * @param regionSelectionPolicy The region selection policy to be used.
	 * @return A pointer to this builder.
	 */
	THBuilder<P, pSize, F, fSize, V, vSize>* setRegionSelectionPolicy(
			RegionSelectionPolicy<P, pSize, F, fSize, V, vSize> *regionSelectionPolicy) {
		if(regionSelectionPolicy != NULL) {
			if(this->regionSelectionPolicy != NULL) delete this->regionSelectionPolicy;
			this->regionSelectionPolicy = regionSelectionPolicy;
		}
		return this;
	}

	/**
	 * @brief Get the SearchSpace configured.
	 *
	 * No search space is configured automatically.
	 *
	 * @return The SearchSpace configured.
	 */
	SearchSpace<P>* getSearchSpace() {
		return searchSpace;
	}

	/**
	 * @brief Set the SearchSpace.
	 *
	 * If a SearchSpace has already been set, it will be deleted
	 * before setting the new instance.
	 *
	 * @param searchSpace The search space to be used.
	 * @return A pointer to this builder.
	 */
	THBuilder<P, pSize, F, fSize, V, vSize>* setSearchSpace(SearchSpace<P> *searchSpace) {
		if(searchSpace != NULL) {
			if(this->searchSpace != NULL) delete this->searchSpace;
			this->searchSpace = searchSpace;
		}
		return this;
	}

	/**
	 * @brief Get the number of startup solutions to be used.
	 * @return The number of startup solutions to be used.
	 */
	int getNStartupSolutions() {
		return nStartupSolutions;
	}

	/**
	 * @brief Get the startup solutions to be used.
	 * @return The startup solutions to be used.
	 */
	Solution<P, pSize, F, fSize, V, vSize>** getStartupSolutions() {
		return startupSolutions;
	}

	/**
	 * @brief Set a single startup solution.
	 *
	 * If set, the root TH instance will use this solution as starting position
	 * for part one population individual.
	 *
	 * @param startupSolution The startup solution to be used.
	 * @return A pointer to this builder.
	 */
	THBuilder<P, pSize, F, fSize, V, vSize>* setStartupSolution(Solution<P, pSize, F, fSize, V, vSize> *startupSolution) {
		Solution<P, pSize, F, fSize, V, vSize> **startupSolutions = new Solution<P, pSize, F, fSize, V, vSize>*[1];
		startupSolutions[0] = startupSolution;
		return setStartupSolutions(startupSolutions, 1);
	}

	/**
	 * @brief Set multiple startup solutions.
	 *
	 * If set, the root TH instance will use these solutions as starting positions
	 * for part of its population (depending on {@link getMaxPopulationSize()}).
	 *
	 * @param startupSolutions The startup solutions to be used.
	 * @param nStartupSolutions The number of startup solutions.
	 * @return A pointer to this builder.
	 */
	THBuilder<P, pSize, F, fSize, V, vSize>* setStartupSolutions(Solution<P, pSize, F, fSize, V, vSize> **startupSolutions, int nStartupSolutions) {
		if(startupSolutions != NULL && nStartupSolutions > 0) {
			if(this->startupSolutions != NULL) {
				for(int i=0; i < this->nStartupSolutions; i++) {
					delete this->startupSolutions[i];
				}
				delete this->startupSolutions;
			}
			this->startupSolutions = startupSolutions;
			this->nStartupSolutions = nStartupSolutions;
		}
		return this;
	}

	long long getMaxNumberEvaluations() {
		return maxNumberEvaluations;
	}

	/**
	 * @brief Set the maximum number of fitness evaluations allowed for this TH instance.
	 * @param maxNumberEvaluations The maximum number of fitness evaluations allowed.
	 * @return A pointer to this builder.
	 */
	THBuilder<P, pSize, F, fSize, V, vSize>* setMaxNumberEvaluations(long long maxNumberEvaluations) {
		this->maxNumberEvaluations = maxNumberEvaluations;
		return this;
	}

	long getMaxTimeSeconds() {
		return maxTimeSeconds;
	}

	/**
	 * @brief Set the maximum execution time (in seconds) allowed for this TH instance.
	 * @param maxTimeSeconds The maximum execution time (in seconds) allowed.
	 * @return A pointer to this builder.
	 */
	THBuilder<P, pSize, F, fSize, V, vSize>* setMaxTimeSeconds(long maxTimeSeconds) {
		this->maxTimeSeconds = maxTimeSeconds;
		return this;
	}

	/**
	 * @brief Get the global number of fitness function evaluations performed
	 *        by this TH instance.
	 * @return The number of fitness function evaluations performed by this TH instance.
	 */
	long long getNEvals(){
		return nEvals;
	}

	int getId() {
		return ID;
	}

	MPI_Comm getCartGrid() {
		return cartGrid;
	}

	long long getMaxIterations() {
		return maxIterations;
	}

	/**
	 * @brief Set the maximum number of TH iterations allowed for this TH instance.
	 * @param maxIterations The maximum number of TH iterations allowed.
	 * @return A pointer to this builder.
	 */
	THBuilder<P, pSize, F, fSize, V, vSize>* setMaxIterations(long long maxIterations) {
		this->maxIterations = maxIterations;
		return this;
	}

	int getBestListSize() {
		return bestListSize;
	}

	/**
	 * @brief Set the best-list size for this TH instance.
	 * @param bestListSize The best-list size.
	 * @return A pointer to this builder.
	 */
	THBuilder<P, pSize, F, fSize, V, vSize>* setBestListSize(int bestListSize) {
		this->bestListSize = bestListSize;
		return this;
	}

	/**
	 * @brief Treasure Hunt's Search Group.
	 */
	class SearchGroup {
		THBuilder<P, pSize, F, fSize, V, vSize> *config;
		THTree *thTree;
		t_node * currNode;
		Region<P> *region;
		vector<SearchScore<P, pSize, F, fSize, V, vSize>*> *searchAlgorithms;
		Search<P, pSize, F, fSize, V, vSize> *searchAlgorithmLastExecuted;
		ConvergenceControlPolicy<P, pSize, F, fSize, V, vSize> *convergenceControlPolicy;
		Solution<P, pSize, F, fSize, V, vSize> **population;
		FitnessPolicy<P, pSize, F, fSize, V, vSize> *fitnessPolicy;

		BestList<P, pSize, F, fSize, V, vSize> *bestList;
		Solution<P, pSize, F, fSize, V, vSize> *iterationBest;
		Solution<P, pSize, F, fSize, V, vSize> *generalBest;
		Solution<P, pSize, F, fSize, V, vSize> *bias;

		int ID;
		int L;
		int n;
		int maxPopulationSize;
		bool improvedGeneralBest;

	public:
		/**
		 * @brief Constructor to create the internal Search Group.
		 * @param config The THBuilder configuration.
		 */
		SearchGroup(THBuilder<P, pSize, F, fSize, V, vSize> *config){
			this->config = config;
			bestList = config->getBestList();
			generalBest = config->getGeneralBest();
			region = config->getSubRegion();

			ID = config->getId();
			thTree = config->getTHTree();
			currNode = thTree->getNode(ID);
			L = currNode->getLevel();
			n = config->getSearchSpace()->getNDimensions();

			searchAlgorithms = config->getSearchAlgorithms();
			if(searchAlgorithms->size() == 0) throw std::invalid_argument("At least one search algorithm must be provided.");
			maxPopulationSize = config->getMaxPopulationSize();
			if(maxPopulationSize <= 0) throw std::invalid_argument("The population size must be greater than zero.");
			convergenceControlPolicy = config->getConvergenceControlPolicy();
			fitnessPolicy = config->getFitnessPolicy();

			// Set the fitness policy to the search algorithm so it can calculate the fitness for every candidate solution.
			for(SearchScore<P, pSize, F, fSize, V, vSize> *sc : *searchAlgorithms){
				Search<P, pSize, F, fSize, V, vSize> *search = sc->getSearchAlgorithm();
				if(search != NULL){
					search->setFitnessPolicy(fitnessPolicy);
					search->setSearchSpace(config->getSearchSpace());
				}
			}

			// Create TH population.
			population = new Solution<P, pSize, F, fSize, V, vSize>*[maxPopulationSize];
			for(int i=0; i < maxPopulationSize; i++){
				population[i] = new Solution<P, pSize, F, fSize, V, vSize>(n);
			}
			iterationBest = new Solution<P, pSize, F, fSize, V, vSize>(n);

			searchAlgorithmLastExecuted = NULL;
			improvedGeneralBest = false;

			// Only the Root level can set the bias.
			bias = NULL;
			if(currNode->isRoot()) {
				bias = config->getBias();
				if(bias != NULL) {
					fitnessPolicy->apply(bias);
					config->getBestListUpdatePolicy()->apply(bestList, bias, fitnessPolicy);
					config->incrementEvals(1);
					DEBUG_INFO("TH[%i] bias was set with fitness = %f.\n", ID, bias->getFitness()->getFirstValue());
					DEBUG2FILE_INFO(ID, "TH[%i] bias was set with fitness = %f.\n", ID, bias->getFitness()->getFirstValue());
				}
			}
		}
		~SearchGroup(){
			for(int i=0; i < maxPopulationSize; i++){
				delete population[i];
			}
			delete population;
			delete iterationBest;
		}

		/**
		 * @brief Perform a complete execution of the search group.
		 */
		void run(){
			DEBUG_TEXT("TH[%i] executing search group...\n", ID);
			DEBUG2FILE_TEXT(ID, "TH[%i] executing search group...\n", ID);
			//DEBUG_SOLUTION_DOUBLE(ID, "Population before optimization", population, getPopulationSize(), n);

			improvedGeneralBest = false;
			Search<P, pSize, F, fSize, V, vSize> *selectedSearchAlgorithm =
					config->getSearchAlgorithmSelectionPolicy()->apply(ID, thTree, searchAlgorithms);
			selectedSearchAlgorithm->setPopulation(population, getPopulationSize());
			convergenceControlPolicy->run(selectedSearchAlgorithm);
			//DEBUG_SOLUTION_DOUBLE(ID, "Population after optimization", population, getPopulationSize(), n);
			config->incrementEvals(selectedSearchAlgorithm->getCurrentNEvals());
			*iterationBest = selectedSearchAlgorithm->getBestIndividual();
			config->getBestListUpdatePolicy()->apply(bestList, iterationBest, fitnessPolicy);
			if(fitnessPolicy->firstIsBetter(iterationBest, generalBest)){
				*generalBest = iterationBest;
				improvedGeneralBest = true;
			}

			config->getSearchAlgorithmSelectionPolicy()->rank(
					ID, thTree, searchAlgorithms,
					selectedSearchAlgorithm,
					iterationBest->getFitness(),
					selectedSearchAlgorithm->getCurrentNEvals(),
					config->getNEvals()
			);

			searchAlgorithmLastExecuted = selectedSearchAlgorithm;

			DEBUG_TEXT("TH[%i] ... search group executed.\n", ID);
			DEBUG2FILE_TEXT(ID, "TH[%i] ... search group executed.\n", ID);
		}

		Solution<P, pSize, F, fSize, V, vSize>** getPopulation() {
			return population;
		}

		int getPopulationSize() {
			return maxPopulationSize;
		}

		/**
		 * @brief Reset the location of the population individuals and calculate the fitness.
		 *
		 * The reset is performed as follows:
		 * - For the root node:
		 *   - If startup Solutions are provided, population individuals will be assigned to these locations;
		 * - For remaining individuals on root node and all other nodes:
		 *   - If a bias is provided:
 		 *     - One single population individuals will be assigned to the bias location (root node only).
		 *     - Half of the remaining individuals will be assigned close to bias location;
		 *     - The other half will be reset within the "anchor" sub-region.
		 *   - If no bias is provided, population individuals will be reset within the "anchor" sub-region.
		 *
		 * @param region The "anchor" sub-region where the population will be reset.
		 */
		void resetPopulation(Region<P> *region) {
			bool hasUsedBias = false;
			unsigned int seed = (bias!=NULL) ? THUtil::getRandomSeed() : 1;
			Solution<P, pSize, F, fSize, V, vSize> **startupSolutions = config->getStartupSolutions();
			int nStartupSolutions = config->getNStartupSolutions();
			for(int i=0; i < maxPopulationSize; i++){
				// For root node, reposition the population members to the startup positions.
				if(currNode->isRoot() && i < nStartupSolutions) {
					*population[i] = startupSolutions[i];
				}
				// If a bias has been provided.
				else if(bias != NULL) {
					// For root node, only 1 individual will be repositioned to bias position.
					if(currNode->isRoot() && !hasUsedBias) {
						hasUsedBias = true;
						*population[i] = bias;
					}
					// 50% of the remaining individuals will be reset closer to the bias.
					else if(THUtil::randUniformDouble(seed, 0, 1) < 0.5)
						population[i]->reset(region, bias);
					// Remaining individuals will be reset within the "anchor" sub-region.
					else population[i]->reset(region);
				}
				// Reposition the population members inside the "anchor" sub-region.
				else population[i]->reset(region);

				fitnessPolicy->apply(population[i]); // Calculate the respective fitness.
				if(i == 0 || fitnessPolicy->firstIsBetter(population[i], iterationBest)){
					*iterationBest = population[i];
				}
			}
			if(fitnessPolicy->firstIsBetter(iterationBest, generalBest)){
				*generalBest = iterationBest;
			}
			config->getBestListUpdatePolicy()->apply(bestList, generalBest, fitnessPolicy);
			config->incrementEvals(maxPopulationSize);
		}

		/**
		 * @brief Get the Search instance of the last optimization algorithm executed.
		 * @return The last optimization algorithm executed.
		 */
		Search<P, pSize, F, fSize, V, vSize>* getSearchAlgorithmLastExecuted(){
			return searchAlgorithmLastExecuted;
		}

		/**
		 * @brief Get the best Solution of the TH iteration.
		 * @return The best Solution of the TH iteration.
		 */
		Solution<P, pSize, F, fSize, V, vSize>* getIterationBest(){
			return iterationBest;
		}

		/**
		 * @brief Inform if the last execution of the {@link run()} method has
		 *        improved the general best Solution.
		 * @return If the general best Solution has improved on last TH iteration.
		 */
		bool hasImprovedGeneralBest(){
			return improvedGeneralBest;
		}
	};

	/**
	 * @brief Actual implementation of Treasure Hunt.
	 */
	class THImpl : public TH<P, pSize, F, fSize, V, vSize> {
		THBuilder *config;
		THTree *thTree;
		t_node * currNode;
		Region<P> *subRegion;
		SearchGroup *searchGroup;
		Search<P, pSize, F, fSize, V, vSize> *localSearchAlgorithm;
		ConvergenceControlPolicy<P, pSize, F, fSize, V, vSize> *convergenceControlPolicy;
		BestList<P, pSize, F, fSize, V, vSize> *bestList, *bestListCopy;
		Solution<P, pSize, F, fSize, V, vSize> **population, *generalBest, *generalBestCopy, *parentBest, *bias;
		FitnessPolicy<P, pSize, F, fSize, V, vSize> *fitnessPolicy;
		IterationData<P, pSize, F, fSize, V, vSize> *iterationData;
		RelocationStrategyData<P, pSize, F, fSize, V, vSize> *relocationStrategyData;

		bool executed;
		struct timeval startTime, currTime;
		int ID, L, parentTH, *childrenTHs, nChildren, *childrenStatuses, *commChildrenStatuses, populationSize, n;
		MPI_Comm cartGrid;
		MPI_Request *reqReadHHbFromParent, *reqSendHbToParent, *reqReadHhFromChildren, *reqSendToChildren, reqReadFinalize;
		P *commSendHbToParent, **commReadHhFromChildren, **Hh, **commSendToChildren, *commReadHHbFromParent, *HHb;
		F *commSendHbFitToParent, *commReadHHbFitFromParent, *HHbFit, *commReadHhFitFromChildren, *commSendFitToChildren, *HhFit;

		long double calcElapsedSeconds(struct timeval startTime, struct timeval endTime){
			return (endTime.tv_sec - startTime.tv_sec) +
				   (endTime.tv_usec - startTime.tv_usec)/1000000.0l;
		}

	public:
		THImpl(THBuilder<P, pSize, F, fSize, V, vSize> *config){
			if(config->getTHTree() == NULL) {
				throw std::invalid_argument("The TH tree must be provided.");
			}
			else if(config->getSearchSpace() == NULL) {
				throw std::invalid_argument("The search space must be provided.");
			}
			else if(config->getFitnessPolicy() == NULL) {
				throw std::invalid_argument("The fitness policy must be provided.");
			}
			else if(config->getCartGrid() == NULL) {
				throw std::invalid_argument("The MPI Cartesian grid must be provided.");
			}
			else if(config->getMaxIterations() == 0 && config->getMaxNumberEvaluations() == 0
					&& config->getMaxTimeSeconds() == 0) {
				throw std::invalid_argument("At least one budget limit must be provided: [iterations, evaluations, seconds].");
			}

			this->config = config;
			executed = false;

			// Tree configuration.
			thTree = config->getTHTree();
			thTree->lock(); // Avoid updates in the tree after TH has begun.
			ID = config->getId();	// ID in the tree.
			currNode = thTree->getNode(ID);
			L = currNode->getLevel();
			n = config->getSearchSpace()->getNDimensions();

			DEBUG_TEXT("TH[%i] located at L[%i].\n", ID, L);
			DEBUG2FILE_TEXT(ID, "TH[%i] located at L[%i].\n", ID, L);
			DEBUG_TEXT_IF(L == thTree->getRootLevel(), "TH tree contains %i nodes.\n", thTree->getCurrentSize());
			DEBUG2FILE_TEXT_IF(ID, L == thTree->getRootLevel(), "TH tree contains %i nodes.\n", thTree->getCurrentSize());

			// Search space configuration.
			subRegion = config->getRegionSelectionPolicy()->apply(		// Partition and select the sub-region.
							config->getSearchSpace(), thTree, ID	// Obtain the full search space.
			);
			config->setSubRegion(subRegion);
			DEBUG_REGION(ID, subRegion);
			DEBUG2FILE_REGION(ID, ID, subRegion);

			// Communication IDs for parent and children.
			parentTH = thTree->getParentID(ID);
			vector<int> children = vector<int>();
			thTree->getChildrenIDs(ID, &children);

			DEBUG_TEXT_IF(L != thTree->getRootLevel(), "TH[%i]'s parent is TH[%i].\n", ID, parentTH);
			DEBUG2FILE_TEXT_IF(ID, L != thTree->getRootLevel(), "TH[%i]'s parent is TH[%i].\n", ID, parentTH);

			// Communication buffers and channels.
			nChildren = children.size();
			if(currNode->hasChildren()){
				childrenTHs = new int[nChildren];
				reqReadHhFromChildren = new MPI_Request[nChildren * 3];
				reqSendToChildren = new MPI_Request[nChildren * 2];
				commReadHhFromChildren = new P*[nChildren];
				commSendToChildren = new P*[nChildren];
				Hh = new P*[nChildren];
				HhFit = new F[nChildren * fSize];
				for(int i=0; i < nChildren; i++){
					childrenTHs[i] = children.at(i);
					commReadHhFromChildren[i] = new P[n * pSize];
					commSendToChildren[i] = new P[n * pSize];
					Hh[i] = new P[n * pSize];
					reqReadHhFromChildren[i*3] = reqReadHhFromChildren[i*3 + 1] = reqReadHhFromChildren[i*3 + 2] = NULL;
					reqSendToChildren[i*2] = reqSendToChildren[i*2 + 1] = NULL;
					reqReadFinalize = NULL;
				}
				commReadHhFitFromChildren = new F[nChildren * fSize];
				commSendFitToChildren = new F[nChildren * fSize];
				commChildrenStatuses = new int[nChildren];
				childrenStatuses = new int[nChildren];
				memset(commChildrenStatuses, 0, nChildren*sizeof(int)); // Initialize Children status with zero.
				memset(childrenStatuses, 0, nChildren*sizeof(int));
			}
			else {
				reqReadHhFromChildren = reqSendToChildren = NULL;
				commReadHhFromChildren = commSendToChildren = Hh = NULL;
				HhFit = commReadHhFitFromChildren = commSendFitToChildren = NULL;
				childrenTHs = childrenStatuses = commChildrenStatuses  = NULL;
			}

			DEBUG_TEXT("TH[%i] contains %i children%s\n", ID, nChildren, (nChildren > 0 ? ": " : "."));
			DEBUG2FILE_TEXT(ID, "TH[%i] contains %i children%s\n", ID, nChildren, (nChildren > 0 ? ": " : "."));
			DEBUG_VECTOR_INT_IF(nChildren > 0, ID, "Child IDs", childrenTHs, nChildren);

			if(currNode->hasParent()){
				HHb = new P[n * pSize];
				commSendHbToParent = new P[n * pSize];
				commReadHHbFromParent = new P[n * pSize];
				reqReadHHbFromParent = new MPI_Request[2];
				reqSendHbToParent = new MPI_Request[3];
				commSendHbFitToParent = new F[fSize];
				commReadHHbFitFromParent = new F[fSize];
				HHbFit = new F[fSize];
				reqReadHHbFromParent[0] = reqReadHHbFromParent[1] = NULL;
				reqSendHbToParent[0] = reqSendHbToParent[1] = reqSendHbToParent[2] = NULL;
			}
			else {
				HHb = commSendHbToParent = commReadHHbFromParent = NULL;
				reqReadHHbFromParent = reqSendHbToParent = NULL;
				commSendHbFitToParent = commReadHHbFitFromParent = HHbFit = NULL;
			}

			bestListCopy = NULL;
			generalBestCopy = NULL;

			// Best solutions.
			bestList = new BestList<P, pSize, F, fSize, V, vSize>(config->getBestListSize(), n);
			generalBest = new Solution<P, pSize, F, fSize, V, vSize>(n);
			parentBest = new Solution<P, pSize, F, fSize, V, vSize>(n);
			fitnessPolicy = config->getFitnessPolicy();
			fitnessPolicy->setWorstFitness(generalBest); // Allow the convergence to occur.

			// Search group configuration.
			config->setBestList(bestList);
			config->setGeneralBest(generalBest);
			searchGroup = new SearchGroup(config);
			searchGroup->resetPopulation(subRegion);
			population = searchGroup->getPopulation(); // Obtain the population created by the search group.
			populationSize = searchGroup->getPopulationSize();
			convergenceControlPolicy = config->getConvergenceControlPolicy();
			localSearchAlgorithm = config->getLocalSearchAlgorithm();
			localSearchAlgorithm->setFitnessPolicy(fitnessPolicy);
			localSearchAlgorithm->setSearchSpace(config->getSearchSpace());
			bias = (currNode->isRoot() ? config->getBias() : NULL); // Only root node has Bias.

			// Configuration for relocation strategy.
			iterationData = new IterationData<P, pSize, F, fSize, V, vSize>(population, populationSize,
					config->getMaxTimeSeconds(), config->getMaxNumberEvaluations(), config->getMaxIterations());
			relocationStrategyData = config->getRelocationStrategyData();
			relocationStrategyData->setIterationData(iterationData);

			// -----------
			// TH startup.
			// -----------

			cartGrid = config->getCartGrid();

			// Start all searches at same point in time, to keep a good cooperation.
			if(thTree->getCurrentSize() > 1) {
				int signal = 1;
				// The leaves unlock the search.
				if(currNode->isLeaf()){
					MPI_Send(&signal, 1, MPI_INT, parentTH, MSG_STARTUP, cartGrid);
					DEBUG_TEXT("TH[%i] sent startup signal to parent TH[%i].\n", ID, parentTH);
					DEBUG2FILE_TEXT(ID, "TH[%i] sent startup signal to parent TH[%i].\n", ID, parentTH);
				}
				else{
					// Parent nodes read startup signal from children.
					for(int i=0; i < nChildren; i++){
						if(MPI_Recv(&childrenStatuses[i], 1, MPI_INT, childrenTHs[i], MSG_STARTUP, cartGrid, MPI_STATUSES_IGNORE) != MPI_SUCCESS) {
							DEBUG_TEXT("TH[%i] error receiving startup signal from child TH[%i].\n", ID, childrenTHs[i]);
							DEBUG2FILE_TEXT(ID, "TH[%i] error receiving startup signal from child TH[%i].\n", ID, childrenTHs[i]);
							exit(1);
						}
						DEBUG_TEXT("TH[%i] received startup signal from child TH[%i].\n", ID, childrenTHs[i])
						DEBUG2FILE_TEXT(ID, "TH[%i] received startup signal from child TH[%i].\n", ID, childrenTHs[i])
					}
					// Non-leaf child nodes send startup signal to parent.
					if(currNode->hasParent()){
						MPI_Send(&signal, 1, MPI_INT, parentTH, MSG_STARTUP, cartGrid);
						DEBUG_TEXT("TH[%i] sent startup signal to parent TH[%i].\n", ID, parentTH)
						DEBUG2FILE_TEXT(ID, "TH[%i] sent startup signal to parent TH[%i].\n", ID, parentTH)
					}
				}
			}

			DEBUG_TEXT("Construction of TH[%i] completed.\n", ID);
			DEBUG2FILE_TEXT(ID, "Construction of TH[%i] completed.\n", ID);
		}
		~THImpl(){
			delete bestList;
			delete generalBest;
			delete parentBest;
			delete iterationData;
			delete config;

			if(currNode->hasChildren()) {
				delete commReadHhFitFromChildren;
				delete commSendFitToChildren;
				delete reqReadHhFromChildren;
				delete reqSendToChildren;
				delete HhFit;
				delete commChildrenStatuses;
				delete childrenStatuses;
				for(int i=0; i < nChildren; i++){
					delete commReadHhFromChildren[i];
					delete commSendToChildren[i];
					delete Hh[i];
				}
				delete commReadHhFromChildren;
				delete commSendToChildren;
				delete Hh;
				delete childrenTHs;
			}

			if(currNode->hasParent()){
				delete HHb;
				delete commSendHbToParent;
				delete commReadHHbFromParent;
				delete reqReadHHbFromParent;
				delete reqSendHbToParent;
				delete commSendHbFitToParent;
				delete commReadHHbFitFromParent;
				delete HHbFit;
			}

			if(subRegion != NULL) delete subRegion;

			delete searchGroup;
		}

		/**
		 * @brief Starts the Treasure Hunt mechanisms.
		 */
		void run(){
			DEBUG_TEXT("Running TH[%i]...\n", ID);
			DEBUG2FILE_TEXT(ID, "Running TH[%i]...\n", ID);

			gettimeofday(&startTime, NULL);
			int commFlag;
			int commStatus = 1;  // Tell to the parent this child TH instance has begun.
			Solution<P, pSize, F, fSize, V, vSize> *childBest = new Solution<P, pSize, F, fSize, V, vSize>(n);
			Solution<P, pSize, F, fSize, V, vSize> *selectedFromBestList =
						new Solution<P, pSize, F, fSize, V, vSize>(config->getBestListSelectionPolicy()->apply(bestList, fitnessPolicy));
			int i, popSeq, t = 1;
			int T = config->getMaxIterations();
			int maxNumberEvaluations = config->getMaxNumberEvaluations();
			int maxTimeSeconds = config->getMaxTimeSeconds();
			bool hasChildrenImproved = false, hasReadValue, runNextIteration;

			do{
				searchGroup->run();

				// -------------------------------
				// If this TH instance has parent.
				// -------------------------------
				// Send the global best to the parent.
				if(currNode->hasParent()){
					if((searchGroup->hasImprovedGeneralBest() || hasChildrenImproved)) {
						if(reqSendHbToParent[0] != NULL) {
							// If previous send has already completed.
							DEBUG_TEXT("TH[%i] checking if parent TH[%i] received the best value sent.\n", ID, parentTH);
							DEBUG2FILE_TEXT(ID, "TH[%i] checking if parent TH[%i] received the best value sent.\n", ID, parentTH);
							if(MPI_Testall(3, reqSendHbToParent, &commFlag, MPI_STATUSES_IGNORE) != MPI_SUCCESS) {
								DEBUG_TEXT("TH[%i] error sending best value to parent TH[%i].\n", ID, parentTH);
								DEBUG2FILE_TEXT(ID, "TH[%i] error sending best value to parent TH[%i].\n", ID, parentTH);
								exit(1);
							}
						}
						else commFlag = 2;
						if(commFlag == 2 || (reqSendHbToParent[0] == MPI_REQUEST_NULL && commFlag)){
							generalBest->getPositions(commSendHbToParent); // Gb positions.
							generalBest->getFitness(commSendHbFitToParent); // Fitness.
							DEBUG_TEXT("TH[%i] trying to send best value to parent TH[%i].\n", ID, parentTH);
							DEBUG2FILE_TEXT(ID, "TH[%i] trying to send best value to parent TH[%i].\n", ID, parentTH);
							MPI_Isend(commSendHbToParent, n * pSize, MpiTypeTraits<P>::GetType(), parentTH, MSG_CHILD2PARENT, cartGrid, &reqSendHbToParent[0]);
							MPI_Isend(commSendHbFitToParent, fSize, MpiTypeTraits<F>::GetType(), parentTH, MSG_CHILD2PARENT, cartGrid, &reqSendHbToParent[1]);
							MPI_Isend(&commStatus, 1, MPI_INT, parentTH, MSG_CHILD2PARENT, cartGrid, &reqSendHbToParent[2]);
							//DEBUG_VECTOR_DOUBLE(ID, "Solution sent to parent", commSendHbToParent, n * pSize);
						}
					}
					else {
						DEBUG_TEXT("TH[%i] no improvement to send to the parent TH[%i].\n", ID, parentTH);
						DEBUG2FILE_TEXT(ID, "TH[%i] no improvement to send to the parent TH[%i].\n", ID, parentTH);
					}
				}

				// ---------------------------------
				// If this TH instance has Children.
				// ---------------------------------
				// Maintain the iteration's best.
				*population[0] = searchGroup->getIterationBest();
				hasChildrenImproved = false;
				popSeq = 1;
				// Send global best to the active children.
				if(currNode->hasChildren()){
					// Communicate with active children.
					for(i=0; i < nChildren && popSeq < populationSize; popSeq++){

						if(childrenStatuses[i] == -2){
							i++; popSeq--;
							continue;
						}
						DEBUG_TEXT("TH[%i]'s child TH[%i] last status is %i.\n", ID, childrenTHs[i], childrenStatuses[i]);
						DEBUG2FILE_TEXT(ID, "TH[%i]'s child TH[%i] last status is %i.\n", ID, childrenTHs[i], childrenStatuses[i]);

						// If there is a previous asynchronous read request for this child.
						if(reqReadHhFromChildren[i*3] != NULL) {
							// Check if previous read has been completed.
							DEBUG_TEXT("TH[%i] checking if best value from child TH[%i] has been read.\n", ID, childrenTHs[i]);
							DEBUG2FILE_TEXT(ID, "TH[%i] checking if best value from child TH[%i] has been read.\n", ID, childrenTHs[i]);
							if(MPI_Testall(3, &reqReadHhFromChildren[i*3], &commFlag, MPI_STATUSES_IGNORE) != MPI_SUCCESS) {
								DEBUG_TEXT("TH[%i] error obtaining best value from child TH[%i].\n", ID, childrenTHs[i]);
								DEBUG2FILE_TEXT(ID, "TH[%i] error obtaining best value from child TH[%i].\n", ID, childrenTHs[i]);
								exit(1);
							}
						}
						else commFlag = 2; // If there is no pending read, force a new read request.

						// If there is a read request for current child, but read has not completed.
						if(!commFlag){
							i++; popSeq--;
							continue;
						}

						// Empty current child's inbound communication channel, given the child could have
						// sent data more than once since last asynchronous read.
						// Only the last read data is maintained.
						hasReadValue = false;
						while(commFlag){
							// If there is a previous asynchronous read request for this child, read the communication buffer.
							if(reqReadHhFromChildren[i*3] == MPI_REQUEST_NULL){
								// The communication buffer must be emptied so it can be reused for the next communication.
								COPY_ARR(&commReadHhFitFromChildren[i * fSize], &HhFit[i * fSize], fSize);
								COPY_ARR(commReadHhFromChildren[i], Hh[i], n * pSize);
								childrenStatuses[i] = commChildrenStatuses[i];
								DEBUG_TEXT("TH[%i] obtained best value from child TH[%i] whose status is now [%i].\n", ID, childrenTHs[i], childrenStatuses[i]);
								DEBUG2FILE_TEXT(ID, "TH[%i] obtained best value from child TH[%i] whose status is now [%i].\n", ID, childrenTHs[i], childrenStatuses[i]);
								//DEBUG_VECTOR_DOUBLE(ID, "Child best value", Hh[i], n * pSize);
								hasReadValue = true;
							}
							// If current child is still active.
							if(childrenStatuses[i] > -2){
								// Issue a new asynchronous read request.
								DEBUG_TEXT("TH[%i] trying to obtain best value from child TH[%i].\n", ID, childrenTHs[i]);
								DEBUG2FILE_TEXT(ID, "TH[%i] trying to obtain best value from child TH[%i].\n", ID, childrenTHs[i]);
								MPI_Irecv(commReadHhFromChildren[i], n * pSize, MpiTypeTraits<P>::GetType(), childrenTHs[i], MSG_CHILD2PARENT, cartGrid, &reqReadHhFromChildren[i*3]);
								MPI_Irecv(&commReadHhFitFromChildren[i * fSize], fSize, MpiTypeTraits<F>::GetType(), childrenTHs[i], MSG_CHILD2PARENT, cartGrid, &reqReadHhFromChildren[i*3+1]);
								MPI_Irecv(&commChildrenStatuses[i], 1, MPI_INT, childrenTHs[i], MSG_CHILD2PARENT, cartGrid, &reqReadHhFromChildren[i*3+2]);
								usleep(10); // Give time for the read request to make effect.

								// Check if previous read request has already completed.
								if(MPI_Testall(3, &reqReadHhFromChildren[i*3], &commFlag, MPI_STATUSES_IGNORE) != MPI_SUCCESS) {
									DEBUG_TEXT("TH[%i] error obtaining best value from child TH[%i].\n", ID, childrenTHs[i]);
									DEBUG2FILE_TEXT(ID, "TH[%i] error obtaining best value from child TH[%i].\n", ID, childrenTHs[i]);
									exit(1);
								}
							}
							else {
								DEBUG_TEXT("TH[%i] child TH[%i] has completed the optimization.\n", ID, childrenTHs[i]);
								DEBUG2FILE_TEXT(ID, "TH[%i] child TH[%i] has completed the optimization.\n", ID, childrenTHs[i]);
								commFlag = 0; // If child has been deactivated, there is no more data to read.
							}
						}

						// In the case the child has not started yet.
						if(childrenStatuses[i] == 0){
							DEBUG_TEXT("TH[%i]'s child TH[%i] has not started yet.\n", ID, childrenTHs[i]);
							DEBUG2FILE_TEXT(ID, "TH[%i]'s child TH[%i] has not started yet.\n", ID, childrenTHs[i]);
							i++; popSeq--;
							continue;
						}

						if(hasReadValue) {
							*childBest = Hh[i];
							childBest->setFitness(&HhFit[i * fSize]);

							// Local search over children's data.
							DEBUG_TEXT("TH[%i]'s performing local search over child's results TH[%i] with fitness %f...\n", ID, childrenTHs[i], childBest->getFitness()->getFirstValue());
							DEBUG2FILE_TEXT(ID, "TH[%i]'s performing local search over child's results TH[%i] with fitness %f...\n", ID, childrenTHs[i], childBest->getFitness()->getFirstValue());
							localSearchAlgorithm->setPopulation(&childBest, 1);
							localSearchAlgorithm->startup();
							localSearchAlgorithm->next(max(convergenceControlPolicy->getBudgetSize()/100, 1));
							config->incrementEvals(localSearchAlgorithm->getCurrentNEvals());
							DEBUG_TEXT("TH[%i]'s local search over child's results TH[%i] performed, obtained fitness %f. Current evals=%ld.\n", ID, childrenTHs[i], childBest->getFitness()->getFirstValue(), (long)config->getNEvals());
							DEBUG2FILE_TEXT(ID, "TH[%i]'s local search over child's results TH[%i] performed, obtained fitness %f. Current evals=%ld.\n", ID, childrenTHs[i], childBest->getFitness()->getFirstValue(), (long)config->getNEvals());
							//DEBUG_SOLUTION_DOUBLE(ID, "Local search performed over child's result", &childBest, 1, n);

							*childBest = localSearchAlgorithm->getBestIndividual();
							if(fitnessPolicy->firstIsBetter(childBest, generalBest)){
								*generalBest = childBest;
								hasChildrenImproved = true;
							}
							config->getBestListUpdatePolicy()->apply(bestList, childBest, fitnessPolicy);

							// Flush the communication data to a population member.
							*population[popSeq] = childBest;
						}
						else popSeq--;

						i++;
					}

					// Select a solution from best list.
					*selectedFromBestList = config->getBestListSelectionPolicy()->apply(bestList, fitnessPolicy);
					// Send the selected solution to all children.
					for(i=0; i < nChildren; i++){
						if(childrenStatuses[i] < 0) continue; // Ignore inactive children.
						// If there is a previous asynchronous send request for this child.
						if(reqSendToChildren[i*2] != NULL) {
							// Check if previous send has been completed.
							DEBUG_TEXT("TH[%i] checking if child TH[%i] received the selected random value from best list.\n", ID, childrenTHs[i]);
							DEBUG2FILE_TEXT(ID, "TH[%i] checking if child TH[%i] received the selected random value from best list.\n", ID, childrenTHs[i]);
							if(MPI_Testall(2, &reqSendToChildren[i*2], &commFlag, MPI_STATUSES_IGNORE) != MPI_SUCCESS) {
								DEBUG_TEXT("TH[%i] error sending a random value from best list to child TH[%i].\n", ID, childrenTHs[i]);
								DEBUG2FILE_TEXT(ID, "TH[%i] error sending a random value from best list to child TH[%i].\n", ID, childrenTHs[i]);
								exit(1);
							}
						}
						else commFlag = 2; // If there is no pending request for send, force a new send request.
						// If all data has been sent to this child, send the currently selected solution from best list.
						if(commFlag == 2 || (reqSendToChildren[i*2] == MPI_REQUEST_NULL && commFlag)){
							selectedFromBestList->getPositions(commSendToChildren[i]);
							selectedFromBestList->getFitness(&commSendFitToChildren[i * fSize]);
							DEBUG_TEXT("TH[%i] trying to send a random value from best list to child TH[%i].\n", ID, childrenTHs[i]);
							DEBUG2FILE_TEXT(ID, "TH[%i] trying to send a random value from best list to child TH[%i].\n", ID, childrenTHs[i]);
							MPI_Isend(commSendToChildren[i], n * pSize, MpiTypeTraits<P>::GetType(), childrenTHs[i], MSG_PARENT2CHILD, cartGrid, &reqSendToChildren[i*2]);
							MPI_Isend(&commSendFitToChildren[i * fSize], fSize, MpiTypeTraits<F>::GetType(), childrenTHs[i], MSG_PARENT2CHILD, cartGrid, &reqSendToChildren[i*2+1]);
							//DEBUG_VECTOR_DOUBLE(ID, "Solution from best-list sent to children", commSendToChildren[i], n * pSize);
						}
					}
				}

				// -------------------------------
				// If this TH instance has Parent.
				// -------------------------------
				if(currNode->hasParent() && t > 1){
					// If there is a previous asynchronous read request for the parent.
					if(reqReadHHbFromParent[0] != NULL) {
						// Check if previous read has been completed.
						DEBUG_TEXT("TH[%i] checking if parent's (TH[%i]) best position has been received.\n", ID, parentTH);
						DEBUG2FILE_TEXT(ID, "TH[%i] checking if parent's (TH[%i]) best position has been received.\n", ID, parentTH);
						if(MPI_Testall(2, reqReadHHbFromParent, &commFlag, MPI_STATUSES_IGNORE) != MPI_SUCCESS) {
							DEBUG_TEXT("TH[%i] error receiving parent's best position from TH[%i].\n", ID, parentTH);
							DEBUG2FILE_TEXT(ID, "TH[%i] error receiving parent's best position from TH[%i].\n", ID, parentTH);
							exit(1);
						}
					}
					else commFlag = 2; // If there is no pending request for read, force a new read request.
					// If data has been read from the parent, make a new request for read.
					hasReadValue = false;
					while(commFlag){
						// If there is a previous asynchronous read request for the parent, read the communication buffer.
						if(reqReadHHbFromParent[0] == MPI_REQUEST_NULL){
							COPY_ARR(commReadHHbFromParent, HHb, n * pSize);
							COPY_ARR(commReadHHbFitFromParent, HHbFit, fSize);
							DEBUG_TEXT("TH[%i] received parent's best position from TH[%i].\n", ID, parentTH);
							DEBUG2FILE_TEXT(ID, "TH[%i] received parent's best position from TH[%i].\n", ID, parentTH);
							//DEBUG_VECTOR_DOUBLE(ID, "Solution received from parent", commReadHHbFromParent, n * pSize);
							hasReadValue = true;
						}
						DEBUG_TEXT("TH[%i] trying to receive parent's best position from TH[%i].\n", ID, parentTH);
						DEBUG2FILE_TEXT(ID, "TH[%i] trying to receive parent's best position from TH[%i].\n", ID, parentTH);
						MPI_Irecv(commReadHHbFromParent, n * pSize, MpiTypeTraits<P>::GetType(), parentTH, MSG_PARENT2CHILD, cartGrid, &reqReadHHbFromParent[0]);
						MPI_Irecv(commReadHHbFitFromParent, fSize, MpiTypeTraits<F>::GetType(), parentTH, MSG_PARENT2CHILD, cartGrid, &reqReadHHbFromParent[1]);
						usleep(10); // Give time for the read request to make effect.
						// If previous receive has already completed.
						if(MPI_Testall(2, reqReadHHbFromParent, &commFlag, MPI_STATUSES_IGNORE) != MPI_SUCCESS) {
							DEBUG_TEXT("TH[%i] error receiving parent's best position from TH[%i].\n", ID, parentTH);
							DEBUG2FILE_TEXT(ID, "TH[%i] error receiving parent's best position from TH[%i].\n", ID, parentTH);
							exit(1);
						}
					}
					if(hasReadValue) {
						*parentBest = HHb;
						parentBest->setFitness(HHbFit);
					}
					else *parentBest = generalBest;
				}
				else{
					*parentBest = generalBest;
				}

				if(bias != NULL && popSeq < populationSize) {
					population[popSeq++]->reset(subRegion, bias);
				}

				// Avoid processing waste on last iteration.
				gettimeofday(&currTime, NULL);
				config->setElapsedSeconds(calcElapsedSeconds(startTime, currTime));
				runNextIteration = (T == 0 || t < T)
									&& (maxNumberEvaluations == 0 || config->getNEvals() < maxNumberEvaluations)
									&& (maxTimeSeconds == 0 || config->getElapsedSeconds() < maxTimeSeconds);
				if(runNextIteration){
					// Save the iteration's data to be used on relocation strategy.
					iterationData->setCurrIteration(t);
					iterationData->setCurrNumberEvaluation(config->getNEvals());
					iterationData->setCurrTime(config->getElapsedSeconds());
					iterationData->setPopulation(population, populationSize);
					iterationData->setGeneralBest(generalBest);
					iterationData->setParentBest(parentBest);
					iterationData->setIterationBest(searchGroup->getIterationBest());

					// Apply the relocation strategy.
					if(popSeq < populationSize) {
						// Perform dynamic region selection.
						subRegion = config->getRegionSelectionPolicy()->recalculate(
											iterationData, config->getSearchSpace(),
											subRegion, thTree, ID
						);

						config->getRelocationStrategyPolicy()->apply(relocationStrategyData, subRegion, &population[popSeq], populationSize-popSeq);

						// Calculate the fitness for the new solutions.
						for(; popSeq < populationSize; popSeq++){
							fitnessPolicy->apply(population[popSeq]);
							config->incrementEvals(1);
						}
						DEBUG_TEXT("TH[%i]'s individuals relocated.\n", ID);
						DEBUG2FILE_TEXT(ID, "TH[%i]'s individuals relocated.\n", ID);
					}
				}
				DEBUG_INFO("TH[%i] Current best solution: [alg=%s, it=%i, evals=%i, currSec=%i, fit=%f]. Iteration's best fit=%f.\n", ID, searchGroup->getSearchAlgorithmLastExecuted()->getName(), t, (int)config->getNEvals(), (int)config->getElapsedSeconds(), generalBest->getFitness()->getFirstValue(), searchGroup->getIterationBest()->getFitness()->getFirstValue());
				DEBUG2FILE_INFO(ID, "TH[%i] Current best solution: [alg=%s, it=%i, evals=%i, currSec=%i, fit=%f]. Iteration's best fit=%f.\n", ID, searchGroup->getSearchAlgorithmLastExecuted()->getName(), t, (int)config->getNEvals(), (int)config->getElapsedSeconds(), generalBest->getFitness()->getFirstValue(), searchGroup->getIterationBest()->getFitness()->getFirstValue());
				DEBUG_TEXT("TH[%i] T=%i, maxNumberEvaluations=%i, maxTimeSeconds=%i, startTime=%i, currTime=%i.\n", ID, T, maxNumberEvaluations, maxTimeSeconds, (int)startTime.tv_sec, (int)currTime.tv_sec);
				DEBUG2FILE_TEXT(ID, "TH[%i] T=%i, maxNumberEvaluations=%i, maxTimeSeconds=%i, startTime=%i, currTime=%i.\n", ID, T, maxNumberEvaluations, maxTimeSeconds, (int)startTime.tv_sec, (int)currTime.tv_sec);

				t++; // Increment the iteration.

			}while(runNextIteration);

			// -----------------------
			// Residual Communication.
			// -----------------------
			DEBUG_TEXT("TH[%i] search phase completed.\n", ID);
			DEBUG2FILE_TEXT(ID, "TH[%i] search phase completed.\n", ID);

			if(currNode->hasParent()){
				// Discard remaining data sent by the parent.
				// From this point on, this sub-tree will focus only in the search intensification.
				if(reqReadHHbFromParent[0] != NULL) {// && reqReadHHbFromParent[0] != MPI_REQUEST_NULL) {
					DEBUG_TEXT("TH[%i] trying to discard parent's data (TH[%i]).\n", ID, parentTH);
					DEBUG2FILE_TEXT(ID, "TH[%i] trying to discard parent's data (TH[%i]).\n", ID, parentTH);
					MPI_Testall(2, reqReadHHbFromParent, &commFlag, MPI_STATUSES_IGNORE);
				}
				else commFlag = 2;
				while(commFlag){
					DEBUG_TEXT("TH[%i] discarding parent's data (TH[%i]).\n", ID, parentTH);
					DEBUG2FILE_TEXT(ID, "TH[%i] discarding parent's data (TH[%i]).\n", ID, parentTH);
					MPI_Irecv(commReadHHbFromParent, n * pSize, MpiTypeTraits<P>::GetType(), parentTH, MSG_PARENT2CHILD, cartGrid, &reqReadHHbFromParent[0]);
					MPI_Irecv(commReadHHbFitFromParent, fSize, MpiTypeTraits<F>::GetType(), parentTH, MSG_PARENT2CHILD, cartGrid, &reqReadHHbFromParent[1]);
					if(MPI_Testall(2, reqReadHHbFromParent, &commFlag, MPI_STATUSES_IGNORE) != MPI_SUCCESS) {
						DEBUG_TEXT("TH[%i] error discarding parent's data (TH[%i]).\n", ID, parentTH);
						DEBUG2FILE_TEXT(ID, "TH[%i] error discarding parent's data (TH[%i]).\n", ID, parentTH);
						exit(1);
					}
					//DEBUG_VECTOR_DOUBLE(ID, "Parent data discarded", commReadHHbFromParent, n * pSize);
				}

				// Inform the parent this TH instance is entering the Residual Communication phase.
				commStatus = -1;
				if(reqSendHbToParent[0] != NULL) {// && reqSendHbToParent[0] != MPI_REQUEST_NULL) {
					DEBUG_TEXT("TH[%i] trying to send best value to parent (TH[%i]).\n", ID, parentTH);
					DEBUG2FILE_TEXT(ID, "TH[%i] trying to send best value to parent (TH[%i]).\n", ID, parentTH);
					MPI_Testall(3, reqSendHbToParent, &commFlag,  MPI_STATUSES_IGNORE);
				}
				else commFlag = 2;
				if(commFlag){ // If parent is not available to receive, send the data later.
					// Send global best to the parent.
					generalBest->getPositions(commSendHbToParent);
					generalBest->getFitness(commSendHbFitToParent);
					DEBUG_TEXT("TH[%i] sending best value to parent (TH[%i]).\n", ID, parentTH);
					DEBUG2FILE_TEXT(ID, "TH[%i] sending best value to parent (TH[%i]).\n", ID, parentTH);
					MPI_Isend(commSendHbToParent, n * pSize, MpiTypeTraits<P>::GetType(), parentTH, MSG_CHILD2PARENT, cartGrid, &reqSendHbToParent[0]);
					MPI_Isend(commSendHbFitToParent, fSize, MpiTypeTraits<F>::GetType(), parentTH, MSG_CHILD2PARENT, cartGrid, &reqSendHbToParent[1]);
					MPI_Isend(&commStatus, 1, MPI_INT, parentTH, MSG_CHILD2PARENT, cartGrid, &reqSendHbToParent[2]);
					//DEBUG_VECTOR_DOUBLE(ID, "Best value sent to parent", commSendHbToParent, n * pSize);
				}
			}

			if(currNode->hasChildren()){
				// Send global best to children.
				for(i=0; i < nChildren; i++){
					if(childrenStatuses[i] < 0) continue; // Ignore inactive children.
					// If there is a previous asynchronous send request for this child.
					if(reqSendToChildren[i*2] != NULL) {// && reqSendToChildren[i*2] != MPI_REQUEST_NULL) {
						// Check if previous send has been completed.
						DEBUG_TEXT("TH[%i] checking if best value has been sent to child TH[%i].\n", ID, childrenTHs[i]);
						DEBUG2FILE_TEXT(ID, "TH[%i] checking if best value has been sent to child TH[%i].\n", ID, childrenTHs[i]);
						if(MPI_Testall(2, &reqSendToChildren[i*2], &commFlag, MPI_STATUSES_IGNORE) != MPI_SUCCESS) {
							DEBUG_TEXT("TH[%i] error sending best value to child TH[%i].\n", ID, childrenTHs[i]);
							DEBUG2FILE_TEXT(ID, "TH[%i] error sending best value to child TH[%i].\n", ID, childrenTHs[i]);
							exit(1);
						}
					}
					else commFlag = 2; // If there is no pending request for send, force a new send request.
					// If all data has been sent to this child, send the global best value.
					if(commFlag){
						generalBest->getPositions(commSendToChildren[i]);
						generalBest->getFitness(&commSendFitToChildren[i * fSize]);
						DEBUG_TEXT("TH[%i] trying to send best value to child TH[%i].\n", ID, childrenTHs[i]);
						DEBUG2FILE_TEXT(ID, "TH[%i] trying to send best value to child TH[%i].\n", ID, childrenTHs[i]);
						MPI_Isend(commSendToChildren[i], n * pSize, MpiTypeTraits<P>::GetType(), childrenTHs[i], MSG_PARENT2CHILD, cartGrid, &reqSendToChildren[i*2]);
						MPI_Isend(&commSendFitToChildren[i * fSize], fSize, MpiTypeTraits<F>::GetType(), childrenTHs[i], MSG_PARENT2CHILD, cartGrid, &reqSendToChildren[i*2+1]);
						//DEBUG_VECTOR_DOUBLE(ID, "General best sent to children", commSendToChildren[i], n * pSize);
					}
				}

				int nInactiveChild = 0;
				Solution<P, pSize, F, fSize, V, vSize> tmpMember(n);
				// Wait all children to finish.
				do{
					usleep(1000000); // Wait 1 second.
					DEBUG_TEXT("TH[%i] has %i children to check.\n", ID, nChildren-nInactiveChild);
					for(nInactiveChild=0, i=0; i < nChildren; i++){
						// Ignore inactive children.
						if(childrenStatuses[i] == -2){
							nInactiveChild++; continue;
						}

						// If there is a previous asynchronous read request for this child.
						if(reqReadHhFromChildren[i*3] != NULL) {// && reqReadHhFromChildren[i*3] != MPI_REQUEST_NULL) {
							DEBUG_TEXT("TH[%i] waiting to hear from its child TH[%i].\n", ID, childrenTHs[i]);
							DEBUG2FILE_TEXT(ID, "TH[%i] waiting to hear from its child TH[%i].\n", ID, childrenTHs[i]);
							// Check if previous read has been completed.
							if(MPI_Testall(3, &reqReadHhFromChildren[i*3], &commFlag, MPI_STATUSES_IGNORE) != MPI_SUCCESS) {
								DEBUG_TEXT("TH[%i] error hearing from its child TH[%i].\n", ID, childrenTHs[i]);
								DEBUG2FILE_TEXT(ID, "TH[%i] error hearing from its child TH[%i].\n", ID, childrenTHs[i]);
								exit(1);
							}
						}
						else commFlag = 2; // If there is no pending request for read, force a new read request.

						// Empty current child's inbound communication channel, given the child could have
						// sent data more than once since last asynchronous read.
						// Only the last read data is maintained.
						hasReadValue = false;
						while(commFlag){
							// If there is a previous asynchronous read request for this child, read the communication buffer.
							if(reqReadHhFromChildren[i*3] == MPI_REQUEST_NULL){
								// The communication buffer must be emptied so it can be reused for the next communication.
								COPY_ARR(commReadHhFromChildren[i], Hh[i], n * pSize);
								COPY_ARR(&commReadHhFitFromChildren[i * fSize], &HhFit[i * fSize], fSize);
								childrenStatuses[i] = commChildrenStatuses[i];
								DEBUG_TEXT("TH[%i] obtained information from child TH[%i].\n", ID, childrenTHs[i]);
								DEBUG2FILE_TEXT(ID, "TH[%i] obtained information from child TH[%i].\n", ID, childrenTHs[i]);
								//DEBUG_VECTOR_DOUBLE(ID, "Child best value", Hh[i], n * pSize);
								hasReadValue = true;
							}
							// If current child is still active.
							if(childrenStatuses[i] > -2){
								// Issue a new asynchronous read request.
								MPI_Irecv(commReadHhFromChildren[i], n * pSize, MpiTypeTraits<P>::GetType(), childrenTHs[i], MSG_CHILD2PARENT, cartGrid, &reqReadHhFromChildren[i*3]);
								MPI_Irecv(&commReadHhFitFromChildren[i * fSize], fSize, MpiTypeTraits<F>::GetType(), childrenTHs[i], MSG_CHILD2PARENT, cartGrid, &reqReadHhFromChildren[i*3+1]);
								MPI_Irecv(&commChildrenStatuses[i], 1, MPI_INT, childrenTHs[i], MSG_CHILD2PARENT, cartGrid, &reqReadHhFromChildren[i*3+2]);
								DEBUG_TEXT("TH[%i] requesting new information from child TH[%i].\n", ID, childrenTHs[i])
								DEBUG2FILE_TEXT(ID, "TH[%i] requesting new information from child TH[%i].\n", ID, childrenTHs[i])
								usleep(10); // Give time for the read request to make effect.
								// Check if previous read request has already completed.
								if(MPI_Testall(3, &reqReadHhFromChildren[i*3], &commFlag, MPI_STATUSES_IGNORE) != MPI_SUCCESS) {
									DEBUG_TEXT("TH[%i] error requesting new information from child TH[%i].\n", ID, childrenTHs[i]);
									DEBUG2FILE_TEXT(ID, "TH[%i] error requesting new information from child TH[%i].\n", ID, childrenTHs[i]);
									exit(1);
								}
							}
							else commFlag = 0; // If child has been deactivated, there is no more data to read.
						}
						if(childrenStatuses[i] == -2){
							nInactiveChild++;
							DEBUG_TEXT("TH[%i]'s child TH[%i] is now inactive.\n", ID, childrenTHs[i]);
							DEBUG2FILE_TEXT(ID, "TH[%i]'s child TH[%i] is now inactive.\n", ID, childrenTHs[i]);
						}
						if(hasReadValue){
							tmpMember = Hh[i];
							tmpMember.setFitness(&HhFit[i * fSize]);
							if(fitnessPolicy->firstIsBetter(&tmpMember, generalBest)){
								DEBUG_TEXT("TH[%i] obtained better information [%f] from child TH[%i].\n", ID, HhFit[i * fSize], childrenTHs[i]);
								DEBUG2FILE_TEXT(ID, "TH[%i] obtained better information [%f] from child TH[%i].\n", ID, HhFit[i * fSize], childrenTHs[i]);
								*generalBest = &tmpMember;

								//----------------
								// Send to parent.
								if(currNode->hasParent()){
									DEBUG_TEXT("TH[%i] checking if parent TH[%i] has read the last information from child TH[%i].\n", ID, parentTH, childrenTHs[i]);
									DEBUG2FILE_TEXT(ID, "TH[%i] checking if parent TH[%i] has read the last information from child TH[%i].\n", ID, parentTH, childrenTHs[i]);
									if(MPI_Testall(3, reqSendHbToParent, &commFlag,  MPI_STATUSES_IGNORE) != MPI_SUCCESS) {
										DEBUG_TEXT("TH[%i] error redirecting child's TH[%i] information to parent TH[%i].\n", ID, childrenTHs[i], parentTH);
										DEBUG2FILE_TEXT(ID, "TH[%i] error redirecting child's TH[%i] information to parent TH[%i].\n", ID, childrenTHs[i], parentTH);
										exit(1);
									}
									if(commFlag){
										generalBest->getPositions(commSendHbToParent);
										generalBest->getFitness(commSendHbFitToParent);
										DEBUG_TEXT("TH[%i] trying to redirect child's TH[%i] information to parent TH[%i].\n", ID, childrenTHs[i], parentTH);
										DEBUG2FILE_TEXT(ID, "TH[%i] trying to redirect child's TH[%i] information to parent TH[%i].\n", ID, childrenTHs[i], parentTH);
										MPI_Isend(commSendHbToParent, n * pSize, MpiTypeTraits<P>::GetType(), parentTH, MSG_CHILD2PARENT, cartGrid, &reqSendHbToParent[0]);
										MPI_Isend(commSendHbFitToParent, fSize, MpiTypeTraits<F>::GetType(), parentTH, MSG_CHILD2PARENT, cartGrid, &reqSendHbToParent[1]);
										MPI_Isend(&commStatus, 1, MPI_INT, parentTH, MSG_CHILD2PARENT, cartGrid, &reqSendHbToParent[2]);
										//DEBUG_VECTOR_DOUBLE(ID, "Child's solution redirected to parent", commSendHbToParent, n * pSize);
									}
								}

								// Send to children.
								if(currNode->hasChildren()){
									for(int j=0; j < nChildren; j++){
										if(j == i || childrenStatuses[j] < 0) continue; // Except to the children that just sent the solution.
										DEBUG_TEXT("TH[%i] checking if last sent information has been read by child TH[%i].\n", ID, childrenTHs[j]);
										DEBUG2FILE_TEXT(ID, "TH[%i] checking if last sent information has been read by child TH[%i].\n", ID, childrenTHs[j]);
										if(MPI_Testall(2, &reqSendToChildren[j*2], &commFlag, MPI_STATUSES_IGNORE) != MPI_SUCCESS) {
											DEBUG_TEXT("TH[%i] error redirecting child's TH[%i] information to child TH[%i].\n", ID, childrenTHs[i], childrenTHs[j]);
											exit(1);
										}
										if(commFlag){
											generalBest->getPositions(commSendToChildren[j]);
											generalBest->getFitness(&commSendFitToChildren[j * fSize]);
											DEBUG_TEXT("TH[%i] trying to redirect child's TH[%i] information to child TH[%i].\n", ID, childrenTHs[i], childrenTHs[j]);
											DEBUG2FILE_TEXT(ID, "TH[%i] trying to redirect child's TH[%i] information to child TH[%i].\n", ID, childrenTHs[i], childrenTHs[j]);
											MPI_Isend(commSendToChildren[j], n * pSize, MpiTypeTraits<P>::GetType(), childrenTHs[j], MSG_PARENT2CHILD, cartGrid, &reqSendToChildren[j*2]);
											MPI_Isend(&commSendFitToChildren[j * fSize], fSize, MpiTypeTraits<F>::GetType(), childrenTHs[j], MSG_PARENT2CHILD, cartGrid, &reqSendToChildren[j*2+1]);
											//DEBUG_VECTOR_DOUBLE(ID, "Child's solution redirected to remaining children", commSendToChildren[j], n * pSize);
										}
									}
								}
							}
						}
					}
				}while(nInactiveChild < nChildren);
			}

			// Print Section.
			//---------------
			DEBUG_TEXT_IF(nChildren>0, "TH[%i]'s children finished. Finishing as well...\n", ID);
			DEBUG2FILE_TEXT_IF(ID, nChildren>0, "TH[%i]'s children finished. Finishing as well...\n", ID);

			// Send the final global best solution to the parent.
			if(currNode->hasParent()){
				// Wait until the parent has read all messages sent by this TH instance.
				if(MPI_Testall(3, reqSendHbToParent, &commFlag, MPI_STATUSES_IGNORE) != MPI_SUCCESS) {
					DEBUG_TEXT("TH[%i] error waiting for parent TH[%i] to read the last package.\n", ID, parentTH);
					DEBUG2FILE_TEXT(ID, "TH[%i] error waiting for parent TH[%i] to read the last package.\n", ID, parentTH);
					exit(1);
				}
				while(!commFlag){
					DEBUG_TEXT("TH[%i] waiting for parent TH[%i] to read the last package.\n", ID, parentTH);
					DEBUG2FILE_TEXT(ID, "TH[%i] waiting for parent TH[%i] to read the last package.\n", ID, parentTH);
					usleep(1000000); // Wait 1 second.
					if(MPI_Testall(3, reqSendHbToParent, &commFlag, MPI_STATUSES_IGNORE) != MPI_SUCCESS) {
						DEBUG_TEXT("TH[%i] error waiting for parent TH[%i] to read the last package.\n", ID, parentTH);
						DEBUG2FILE_TEXT(ID, "TH[%i] error waiting for parent TH[%i] to read the last package.\n", ID, parentTH);
						exit(1);
					}
				}
				generalBest->getPositions(commSendHbToParent);
				generalBest->getFitness(commSendHbFitToParent);
				DEBUG_TEXT("TH[%i] Trying to send last best value and inform to parent TH[%i] that this instance has finished.\n", ID, parentTH);
				DEBUG2FILE_TEXT(ID, "TH[%i] Trying to send last best value and inform to parent TH[%i] that this instance has finished.\n", ID, parentTH);
				commStatus = -2; // Notify the parent this TH instance is shutting down.
				MPI_Isend(commSendHbToParent, n * pSize, MpiTypeTraits<P>::GetType(), parentTH, MSG_CHILD2PARENT, cartGrid, &reqSendHbToParent[0]);
				MPI_Isend(commSendHbFitToParent, fSize, MpiTypeTraits<F>::GetType(), parentTH, MSG_CHILD2PARENT, cartGrid, &reqSendHbToParent[1]);
				MPI_Isend(&commStatus, 1, MPI_INT, parentTH, MSG_CHILD2PARENT, cartGrid, &reqSendHbToParent[2]);
				DEBUG_TEXT("TH[%i] Sent last best value to parent TH[%i].\n", ID, parentTH);
				DEBUG2FILE_TEXT(ID, "TH[%i] Sent last best value to parent TH[%i].\n", ID, parentTH);
			}

			// Wait the children to read all data packages sent.
			if(currNode->hasChildren()){
				for(i=0; i < nChildren; i++){
					DEBUG_TEXT("TH[%i] waiting for child TH[%i] to read the last package.\n", ID, childrenTHs[i]);
					DEBUG2FILE_TEXT(ID, "TH[%i] waiting for child TH[%i] to read the last package.\n", ID, childrenTHs[i]);
					MPI_Waitall(2, &reqSendToChildren[i*2], MPI_STATUSES_IGNORE);
					DEBUG_TEXT("TH[%i]'s child TH[%i] did read all the packages.\n", ID, childrenTHs[i]);
					DEBUG2FILE_TEXT(ID, "TH[%i]'s child TH[%i] did read all the packages.\n", ID, childrenTHs[i]);
				}
			}

			// ----------------------
			// Finalize the sub-tree.
			// ----------------------

			// Wait for parent's finalization signal.
			int signal = 0;
	        if(currNode->hasParent()){
	        	while(!signal) {
	    			// Discard remaining parent data (starting by leaf nodes).
					if(MPI_Testall(2, reqReadHHbFromParent, &commFlag, MPI_STATUSES_IGNORE) != MPI_SUCCESS) {
						DEBUG_TEXT("TH[%i] error discarding parent's data TH[%i].\n", ID, parentTH);
						DEBUG2FILE_TEXT(ID, "TH[%i] error discarding parent's data TH[%i].\n", ID, parentTH);
						exit(1);
					}
					while(commFlag){
						MPI_Irecv(commReadHHbFromParent, n, MPI_DOUBLE, parentTH, MSG_PARENT2CHILD, cartGrid, &reqReadHHbFromParent[0]);
						MPI_Irecv(commReadHHbFitFromParent, 1, MPI_DOUBLE, parentTH, MSG_PARENT2CHILD, cartGrid, &reqReadHHbFromParent[1]);
						DEBUG_TEXT("TH[%i] discarding parent's data TH[%i].\n", ID, parentTH);
						DEBUG2FILE_TEXT(ID, "TH[%i] discarding parent's data TH[%i].\n", ID, parentTH);
						usleep(1000000);
						if(MPI_Testall(2, reqReadHHbFromParent, &commFlag, MPI_STATUSES_IGNORE) != MPI_SUCCESS) {
							DEBUG_TEXT("TH[%i] error discarding parent's data TH[%i].\n", ID, parentTH);
							DEBUG2FILE_TEXT(ID, "TH[%i] error discarding parent's data TH[%i].\n", ID, parentTH);
							exit(1);
						}
					}

	    			DEBUG_TEXT("TH[%i] waiting for finalization signal from parent TH[%i].\n", ID, parentTH);
					DEBUG2FILE_TEXT(ID, "TH[%i] waiting for finalization signal from parent TH[%i].\n", ID, parentTH);
					MPI_Irecv(&signal, 1, MPI_INT, parentTH, MSG_FINALIZE, cartGrid, &reqReadFinalize);
					DEBUG_TEXT("TH[%i] received finalization signal from parent TH[%i].\n", ID, parentTH);
					DEBUG2FILE_TEXT(ID, "TH[%i] received finalization signal from parent TH[%i].\n", ID, parentTH);
					if(!signal) usleep(1000000); // Wait 1 second.
	        	}
			}
			// Send finalization signal to children, starting from root node.
	        signal = MSG_FINALIZE;
			if(currNode->hasChildren()){
                for(i=0; i < nChildren; i++){
                	DEBUG_TEXT("TH[%i] sending finalization signal to child TH[%i].\n", ID, childrenTHs[i]);
                	DEBUG2FILE_TEXT(ID, "TH[%i] sending finalization signal to child TH[%i].\n", ID, childrenTHs[i]);
					MPI_Send(&signal, 1, MPI_INT, childrenTHs[i], MSG_FINALIZE, cartGrid);
                	DEBUG_TEXT("TH[%i] sent finalization signal to child TH[%i].\n", ID, childrenTHs[i]);
                	DEBUG2FILE_TEXT(ID, "TH[%i] sent finalization signal to child TH[%i].\n", ID, childrenTHs[i]);
				}
			}

			if(thTree->getCurrentSize() > 1) {
				// Leaf nodes reply the confirmation for the finalization signal.
				if(currNode->isLeaf()){
					DEBUG_TEXT("TH[%i] (leaf) sending back confirmation of finalization signal to parent TH[%i].\n", ID, parentTH);
					DEBUG2FILE_TEXT(ID, "TH[%i] (leaf) sending back confirmation of finalization signal to parent TH[%i].\n", ID, parentTH);
					MPI_Send(&signal, 1, MPI_INT, parentTH, MSG_FINALIZE, cartGrid);
					DEBUG_TEXT("TH[%i] (leaf) confirmation of finalization signal sent back to parent TH[%i].\n", ID, parentTH);
					DEBUG2FILE_TEXT(ID, "TH[%i] (leaf) confirmation of finalization signal sent back to parent TH[%i].\n", ID, parentTH);
				}
				else{
					// Parent nodes read children's confirmation for the finalization signal.
					for(i=0; i < nChildren; i++){
						DEBUG_TEXT("TH[%i] receiving confirmation of finalization signal from child TH[%i].\n", ID, childrenTHs[i]);
						DEBUG2FILE_TEXT(ID, "TH[%i] receiving confirmation of finalization signal from child TH[%i].\n", ID, childrenTHs[i]);
						MPI_Recv(&signal, 1, MPI_INT, childrenTHs[i], MSG_FINALIZE, cartGrid, MPI_STATUSES_IGNORE);
						DEBUG_TEXT("TH[%i] received confirmation of finalization signal from child TH[%i].\n", ID, childrenTHs[i]);
						DEBUG2FILE_TEXT(ID, "TH[%i] received confirmation of finalization signal from child TH[%i].\n", ID, childrenTHs[i]);
					}
					// Parent nodes reply the confirmation for the finalization signal.
					if(currNode->hasParent()){
						DEBUG_TEXT("TH[%i] sending back confirmation of finalization signal to parent TH[%i].\n", ID, parentTH);
						DEBUG2FILE_TEXT(ID, "TH[%i] sending back confirmation of finalization signal to parent TH[%i].\n", ID, parentTH);
						MPI_Send(&signal, 1, MPI_INT, parentTH, MSG_FINALIZE, cartGrid);
						DEBUG_TEXT("TH[%i] confirmation of finalization signal sent back to parent TH[%i].\n", ID, parentTH);
						DEBUG2FILE_TEXT(ID, "TH[%i] confirmation of finalization signal sent back to parent TH[%i].\n", ID, parentTH);
					}
				}
			}

			executed = true;
			DEBUG_TEXT("TH[%i] execution finished.\n", ID);
			DEBUG2FILE_TEXT(ID, "TH[%i] execution finished.\n", ID);
			delete childBest;
			delete selectedFromBestList;
		}

		/**
		 * @brief Get the TH instance's unique ID in the processing grid.
		 * @return TH instance's unique ID.
		 */
		int getID() {
			return config->getId();
		}

		/**
		 * @brief Get a copy of the global best solution.
		 *
		 * The caller is responsible for freeing up (deleting) the memory for this copy.
		 * If called before the completion of the search, this method returns NULL.
		 *
		 * @return A copy of the global best solution.
		 */
		Solution<P, pSize, F, fSize, V, vSize>* getBestSolution(){
			if(executed && generalBestCopy == NULL){
				generalBestCopy = new Solution<P, pSize, F, fSize, V, vSize>(generalBest);
			}
			return generalBestCopy;
		}

		/**
		 * @brief Get a copy of the best-list.
		 *
		 * The caller is responsible for freeing up (deleting) the memory for this copy.
		 * If called before the completion of the search, this method returns NULL.
		 *
		 * @return A copy of the best-list.
		 */
		BestList<P, pSize, F, fSize, V, vSize>* getBestList(){
			if(executed && bestListCopy == NULL){
				bestListCopy = new BestList<P, pSize, F, fSize, V, vSize>(bestList);
			}
			return bestListCopy;
		}

		/**
		 * @brief Get the total number of fitness function evaluations.
		 * @return The total number of fitness function evaluations.
		 */
		long long getNEvals(){
			return config->getNEvals();
		}
	};
};

#endif /* THBUILDER_H_ */

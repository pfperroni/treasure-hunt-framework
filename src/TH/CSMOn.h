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
 * @file CSMOn.h
 * @class CSMOn
 * @author Peter Frank Perroni
 * @brief This policy runs, monitors and controls the convergence limits
 *        for the current TH iteration.
 * @note For the CSMOn (formerly C'MOn) method, please refer (and cite) to the following paper:\n
 *   <em>PERRONI, Peter Frank; WEINGAERTNER, Daniel; DELGADO, Myriam Regattieri.
 *   Estimating stop conditions of swarm based stochastic metaheuristic algorithms.
 *   In: Proceedings of the Genetic and Evolutionary Computation Conference. 2017.
 *   p. 43-50.</em>
 */

#ifndef CSMON_H_
#define CSMON_H_

#include "ConvergenceControlPolicy.h"
#include <vector>
#include <cmath>

template <class F = double>
struct t_point{
	int x;
	F y;
	t_point(int _x, F _y){
		x = _x, y = _y;
	}
};

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class CSMOn : public ConvergenceControlPolicy<P, pSize, F, fSize, V, vSize> {
	int M;
	int s;
	double R;
	F minEstimatedFit;
	vector<t_point<F>> *gb;

	int adjustExp(Search<P, pSize, F, fSize, V, vSize> *search, double r) {
		int sPrev = s;
		getBest(search, 2);
		if(s-sPrev < 2) return -1;
		int pB = -1;
		double alpha1, alpha2;
		while(search->getCurrentNEvals() < M && !search->isStuck()){
			if(decayE() < r && decayL() < r){
				if(pB == -1){
					pB = s-2;
					alpha2 = alphaE(pB, s);
				}
				else{
					alpha1 = alpha2;
					alpha2 = alphaE(pB, s);
					if(alpha2 < alpha1) return s;
				}
			}
			else pB = -1;
			getBest(search, 1);
		}
		return -1;
	}

	int adjustLog(Search<P, pSize, F, fSize, V, vSize> *search, double r, int pT) {
		int sPrev = s;
		getBest(search, 3);
		if(s-sPrev < 3) return -1;
		double alpha1 = alphaP(pT, s-1);
		double alpha2 = alphaP(pT, s);
		while(alpha2 >= alpha1 && search->getCurrentNEvals() < M && !search->isStuck()){
			if(decayE() >= r || decayL() >= r) return -1;
			getBest(search, 1);
			alpha1 = alpha2;
			alpha2 = alphaP(pT, s);
		}
		return s;
	}

	void getBest(Search<P, pSize, F, fSize, V, vSize> *search, int nBest) {
		for(int i=0; i < nBest && search->getCurrentNEvals() < M && !search->isStuck(); i++){
			search->next(M);
			gb->push_back(t_point<F>(search->getCurrentNEvals(), search->getBestFitness()->getFirstValue()));
			s++;
		}
	}

	double decayE() {
		return fabs(1 - ((*gb)[s].y-minEstimatedFit) / ((*gb)[s-1].y-minEstimatedFit));
	}

	double decayL() {
		return fabs(1 - ((*gb)[s].y-(*gb)[s-1].y) / ((*gb)[s-1].y-(*gb)[s-2].y));
	}

	double alphaE(int p1, int p2) {
		int i, n = p2 - p1 + 1;
		double yAvg = 0, yAvgLn, ySumLn = 0, xSum = 0, xAvg, S1 = 0, S2 = 0, aux;
		for(i=p1; i <= p2; i++){
			xSum += (*gb)[i].x;
			yAvg += (*gb)[i].y;
			ySumLn += log((*gb)[i].y);
		}
		xAvg = xSum / n;
		yAvg /= n;
		yAvgLn = ySumLn / n;
		for(i=p1; i <= p2; i++){
			aux = ((*gb)[i].x - xAvg);
			S1 += aux * ((*gb)[i].y - yAvgLn);
			S2 += aux * aux;
		}
		return (ySumLn - (S1 / S2) * xSum) / n;
	}

	double alphaP(int p1, int p2) {
		int i, n = p2 - p1 + 1;
		double yAvgLog, ySumLog = 0, xAvgLog, xSumLog = 0, S1 = 0, S2 = 0, aux;
		for(i=p1; i <= p2; i++){
			xSumLog += log10((*gb)[i].x);
			ySumLog += log10((*gb)[i].y);
		}
		xAvgLog = xSumLog / n;
		yAvgLog = ySumLog / n;
		for(i=p1; i <= p2; i++){
			aux = (log10((*gb)[i].x) - xAvgLog);
			S1 += aux * (log10((*gb)[i].y) - yAvgLog);
			S2 += aux * aux;
		}
		return (ySumLog - (S1 / S2) * xSumLog) / n;
	}

public:
	/**
	 * @brief This constructor creates an instance of CSMOn.
	 *
	 * @param M The maximum number of fitness function evaluations allowed.
	 * @param R The relaxation factor between ]0, 1[ that will regulate the acceptance of the convergence stabilization.
	 *          Larger factors will stop the optimization sooner.
	 * @param minEstimatedFit The minimum estimated fitness for the problem being optimized (fitness function dependent).
	 */
	CSMOn(int M, double R, F minEstimatedFit) : ConvergenceControlPolicy<P, pSize, F, fSize, V, vSize>(M) {
		this->M = this->getBudgetSize();
		this->R = R;
		this->minEstimatedFit = minEstimatedFit;
		gb = new vector<t_point<F>>();
		s = -1;
	}
	~CSMOn() {
		delete gb;
	}

	/**
	 * @brief This method runs, monitors and limits the convergence of the optimization method
	 * during the current TH iteration.
	 *
	 * @param search The optimization method to run.
	 */
	void run(Search<P, pSize, F, fSize, V, vSize> *search) {
		s = -1;
		gb->clear();
		search->startup();

		int pT = -1, pS = -1;
		double r = 0.99;
		getBest(search, 1);
		do{
			r = max(r*r, R);
			if(pS == -1)
				pT = adjustExp(search, r);
			if(pT > 0)
				pS = adjustLog(search, r, pT);
		}while(search->getCurrentNEvals() < M && (r > R || pS == -1) && !search->isStuck());

		search->finalize();
	}
};

#endif /* CSMON_H_ */

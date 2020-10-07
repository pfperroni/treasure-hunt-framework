/**
 * Treasure Hunt Framework (c)
 *
 * Copyright 2016-2020 Peter Frank Perroni
 *
 * @file GroupRegionSelectionPolicy.h
 * @class GroupRegionSelectionPolicy
 * @author Peter Frank Perroni
 * @brief This policy implements the sub-region selection criteria by grouping dimensions.
 * @details All dimensions inside the same group will be partitioned proportionally
 *          at same positions, and then an "anchor" sub-region is selected based on
 *          TH instance's position on THTree topology.
 */

#ifndef TH_GROUPREGIONSELECTIONPOLICY_H_
#define TH_GROUPREGIONSELECTIONPOLICY_H_

#include "RegionSelectionPolicy.h"
#include <cmath>
#include <cstring>

template <class P = double, int pSize = 1, class F = double, int fSize = 1, class V = double, int vSize = 1>
class GroupRegionSelectionPolicy : public RegionSelectionPolicy<P, pSize, F, fSize, V, vSize> {
	int nGroups;
	int K;

	Region<P>* internalLoop(Region<P> *region, vector<int> *hierarchy, t_node *node, int ID) {
		if(node->getID() == ID){ // If the recursive iteration found the ID's sub-region.
			return new Region<P>(region);
		}
		// Obtains the correct subdivision of node's search space by finding
		// the node's child that is in the top of ID's parentage.
		std::vector<t_node*> *children = node->getChildren();
		for(int childPos=0, top=hierarchy->size()-1; childPos < (int)children->size(); childPos++) {
			// If this child is in top of ID's parentage.
			if((*children)[childPos]->getID() == (*hierarchy)[top]){
				// Find child's coordinate in dimension grouping.
				int coord[nGroups];
				memset(coord, 0, sizeof(int)*nGroups);
				for(int pos=childPos, g=nGroups-1, base; g >= 0; g--) {
					base = pow(K, g);
					if(base <= pos) {
						coord[g] = pos / base; // Simplified coordinate in group g.
						pos %= base;
					}
				}
				// Convert: child's coordinate in dimension grouping -> search space boundaries inside parent's subregion.
				int nDim = region->getNDimensions();
				int dimPerGroup = nDim / nGroups;
				Region<P> subRegion(region);
				Partition<P>* partition;
				P delta, minimum;
				for(int d=0, g=0; d < nDim; d++){
					partition = (*region)[d]; // Perform the partitioning using the sequential order of dimension's ID.
					delta = (partition->getEndPoint() - partition->getStartPoint()) / K;
					minimum = partition->getStartPoint() + coord[g] * delta;
					subRegion[d]->setStartPoint(minimum);
					subRegion[d]->setEndPoint((coord[g]<K-1) ? minimum + delta : partition->getEndPoint());
					if((d+1) % dimPerGroup == 0) g++; // Move to the next group.
				}
				hierarchy->pop_back(); // Remove the node that is in the top of ID's parentage.
				// Re-partition the just found sub-region according to ID's descendancy.
				return internalLoop(&subRegion, hierarchy, (*children)[childPos], ID);
			}
		}

		return NULL;
	}

public:
	/**
	 * @brief Constructor that creates a GroupRegionSelectionPolicy instance.
	 * @param nGroups The number of groups to organize the dimensions.
	 * @param K The number of segments to partition every group.
	 */
	GroupRegionSelectionPolicy(int nGroups, int K) {
		this->nGroups = nGroups;
		this->K = K;
	}
	~GroupRegionSelectionPolicy() {}

	/**
	 * @brief Method responsible for choosing one "anchor" sub-region according to the TH instance's ID.
	 *
	 * The caller is responsible for deleting (freeing up the memory of) the Region instance returned.
	 *
	 * @param S The search space to be partitioned.
	 * @param tree The tree topology that will guide the partitioning.
	 * @param ID The TH instance's global ID.
	 * @return The "anchor" sub-region for current TH instance.
	 */
	Region<P>* apply(SearchSpace<P> *S, THTree *thTree, int ID) {
		vector<int> hierarchy = vector<int>();
		t_node *node = thTree->getNode(ID); // Get the current node ID.
		if(node == NULL) return NULL;

		hierarchy.push_back(node->getID()); // Add the current node to the hierarchy.
		t_node *root = thTree->getRootNode();
		// Add all except the root node the current node's hierarchy.
		for(t_node *parent=node->getParent(); parent != NULL && parent != root; parent=parent->getParent()) {
			hierarchy.push_back(parent->getID());
		}

		// Find the current node's sub-region,
		// starting the recursive search from root's search space.
		return internalLoop(S, &hierarchy, root, ID);
	}
};

#endif /* TH_GROUPREGIONSELECTIONPOLICY_H_ */

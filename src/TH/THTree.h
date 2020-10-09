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
 * @file THTree.h
 * @class THTree
 * @author Peter Frank Perroni
 * @brief Treasure Hunt Tree Topology.
 * @details The communication topology between TH instances must be defined by a THTree instance,
 *          where every tree node has its own unique ID, that must match the TH instance ID
 *          according to its position in the processing grid.
 *
 *          Therefore, to change TH instance's position in the topology,
 *          THTree's node ID must be adjusted accordingly.
 *
 *          Notice it is mandatory to {@link lock()} the THTree topology before using it.
 */

#ifndef TH_THTREE_H_
#define TH_THTREE_H_

#include <map>
#include <vector>
#include <cstdlib>

struct t_node {
	t_node *parent;
	int ID;
	int L;
	std::vector<t_node*> children;

public:

	t_node() {
		parent = NULL;
		ID = L = -1;
	}
	std::vector<t_node*>* getChildren() {
		return &children;
	}

	int getNChildren() {
		return children.size();
	}

	void addChildren(t_node *child) {
		children.push_back(child);
	}

	bool hasChildren() {
		return children.size() > 0;
	}

	bool hasParent() {
		return parent != NULL;
	}

	bool isLeaf() {
		return ! hasChildren();
	}

	bool isRoot() {
		return ! hasParent();
	}

	int getID() {
		return ID;
	}

	void setID(int ID) {
		this->ID = ID;
	}

	int getLevel() {
		return L;
	}

	void setLevel(int L) {
		this->L = L;
	}

	t_node* getParent() {
		return parent;
	}

	void setParent(t_node *parent) {
		this->parent = parent;
	}
};

class THTree {
	bool locked;
	int limitSize, currSize;
	int LRoot;
	t_node **nodes, *root;
	std::map<int, t_node*> nodeMap;

	void pack(t_node *node, int L) {
		node->setLevel(L);
		for(t_node *child : *node->getChildren()) {
			pack(child, L-1);
		}
	}

	static std::stringstream print_loop(t_node * node) {
		std::stringstream ss;
		ss << "[ {" << node->getID() << ", " << node->getLevel() << "} ";
		if(node->getNChildren() > 0) {
			for(t_node *child : *node->getChildren()) {
				ss << print_loop(child).str();
			}
		}
		ss << "] ";
		return ss;
	}

public:
	/**
	 * @brief Constructor to create a tree topology with a fixed number of nodes.
	 * @param limitSize The fixed size of the tree topology.
	 */
	THTree(int limitSize) {
		if(limitSize <= 0) throw std::invalid_argument("The tree size must be greater than zero.");
		this->limitSize = limitSize;
		nodes = new t_node*[limitSize];
		for(int i=0; i < limitSize; i++){
			nodes[i] = new t_node;
		}
		currSize = 0;
		root = NULL;
		LRoot = 1;
		locked = false;
	}
	~THTree(){
		for(int i=0; i < limitSize; i++){
			delete nodes[i];
		}
		delete nodes;
	}

	/**
	 * @brief Add the Root node to the tree topology.
	 * @param ID The actual ID of the TH root instance in the grid.
	 * @return This object.
	 */
	THTree* addRootNode(int ID) {
		if(locked) throw std::invalid_argument("The tree is locked and cannot be changed.");
		if(root != NULL) throw std::invalid_argument("Root node already exists.");
		LRoot = 1;
		root = nodes[0];
		root->setID(ID);
		root->setLevel(LRoot);
		nodeMap.insert({ID, root});
		currSize = 1;
		return this;
	}

	/**
	 * @brief Add a child node to the tree topology.
	 * @param ID The actual ID of the TH child instance in the grid.
	 * @param parentID The actual ID of the TH parent instance in the grid.
	 * @return This object.
	 */
	THTree* addNode(int ID, int parentID) {
		if(locked) throw std::invalid_argument("The tree is locked and cannot be changed.");
		if(currSize >= limitSize) throw std::invalid_argument("Tree limit size reached.");
		t_node *parent = nodeMap.at(parentID);
		if(parent == NULL) throw std::invalid_argument("Invalid parent node.");
		t_node *currNode = nodes[currSize];
		currNode->setParent(parent);
		currNode->setID(ID);
		currNode->setLevel(parent->getLevel() + 1);
		if(currNode->getLevel() > LRoot) LRoot = currNode->getLevel();
		nodeMap.insert({ID, currNode});
		parent->addChildren(currNode);
		currSize++;
		return this;
	}

	/**
	 * @brief Lock this THTree topology for any further change.
	 *
	 * The lock is Mandatory before using the topology since
	 * it will pack internal references.
	 */
	void lock(){
		locked = true;
		// Pack the tree level.
		if(root->getLevel() != LRoot) pack(root, LRoot);
	}

	t_node* getRootNode() {
		return root;
	}

	t_node* getNode(int ID) {
		return nodeMap.at(ID);
	}

	int getRootLevel() {
		return LRoot;
	}

	int getLevel(int ID) {
		t_node* node = getNode(ID);
		if(node != NULL) return node->getLevel();
		return -1;
	}

	t_node* getParent(int ID) {
		t_node* node = getNode(ID);
		if(node != NULL) return node->getParent();
		return NULL;
	}

	int getParentID(int ID) {
		t_node* parent = getParent(ID);
		if(parent != NULL) return parent->getID();
		return -1;
	}

	std::vector<t_node*>* getChildren(int ID) {
		t_node* node = getNode(ID);
		if(node != NULL) return node->getChildren();
		return NULL;
	}

	void getChildrenIDs(int ID, std::vector<int>* IDs) {
		if(IDs == NULL) return;
		std::vector<t_node*> *children = getChildren(ID);
		for(t_node* child : *children) {
			IDs->push_back(child->getID());
		}
	}

	/**
	 * @brief Get the tree topology size.
	 * @return The number of nodes in the tree.
	 */
	int getCurrentSize() {
		return currSize;
	}

	void print() {
		print(root);
	}

	static void print(t_node * node) {
		cout << print_loop(node).str().c_str() << endl;
	}
};

#endif /* TH_THTREE_H_ */

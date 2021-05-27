#pragma once

/******************************************************************************
* INCLUDE HEADER FILES
******************************************************************************/
#include <iostream>
#include "Tx.h"
#include "MerkleNode.h"

/******************************************************************************
* CLASS TEMPLATE MERKLE TREE
******************************************************************************/
template <std::string(*hashFunc)(std::string&)> class MerkleTree
{

public:

    MerkleTree();
	MerkleTree(std::vector <std::string>& txIds);
    ~MerkleTree();
	std::string getMerkleRoot(void);
    MerkleNode* getRootNode(void);
	void getMerklePath(std::string& txid, std::vector<std::string>& merklepath);

private:

    MerkleNode* root;
    std::vector<MerkleNode*> blocks;

};
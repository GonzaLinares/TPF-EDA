#pragma once

#include <iostream>
#include "Tx.h"
#include "MerkleNode.h"

//TODO: arreglar la prolijidad

template <std::string(*hashFunc)(std::string&)> class MerkleTree
{

public:

    MerkleTree();
	MerkleTree(std::vector <std::string>& txIds);
    ~MerkleTree();
	std::string getMerkleRoot(void);
	void getMerklePath(std::string& txid, std::vector<std::string>& merklepath);

private:

    MerkleNode* root;
    std::vector<MerkleNode*> blocks;

};
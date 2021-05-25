#pragma once

#include <iostream>
#include "Tx.h"
#include "MerkleNode.h"

template <std::string (*hashFunc)(std::string&)> class MerkleTree
{
	
public:

	MerkleTree(std::vector <std::string> txIds);
	std::string getMerkleRoot(void);
	void getMerklePath(std::string txid, std::vector<std::string> merklepath);

private:

	MerkleNode* root;

};

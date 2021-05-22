#pragma once

#include "Tx.h"

#include <iostream>
#include <vector>

class Block
{

public:

	//Constructor
	Block(std::string jsonBlock);

	//Getters

	void getMerklePath(void);
	void getMerkleRoot(void);

	//Setters



private:

	std::string blockId;
	unsigned int height;
	unsigned int nonce;
	std::string previousBlockId;
	std::string merkleRoot;
	std::vector<Tx> txs;

};


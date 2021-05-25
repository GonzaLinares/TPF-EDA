#pragma once

#include "Tx.h"

#include <iostream>
#include <vector>

class Block
{

public:

	//Constructor
	Block(std::string blockid_, unsigned int height_, std::string merkleRoot_, unsigned int nTx_, int nonce_, std::string prevblockid_);

	//Getters
	bool push_transaction(Tx);
	void getMerklePath(void);		//TODO: Que reciba txid
	void getMerkleRoot(void);
	std::string getBlockId(void);

	//Setters



private:

	std::string blockId;
	unsigned int height;
	unsigned int nonce;
	std::string previousBlockId;
	std::string merkleRoot;
	std::vector<Tx> txs;

};


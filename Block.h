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
	std::string getMerkleRoot(void);


	std::string getId(void);
	unsigned int getHeight(void);
	unsigned int getNonce(void);
	std::string getPrevBlockId(void);
	unsigned int getnTx(void);

	bool getTxsID(std::vector<std::string>& buffer, int numOfTxs = 0, int offset = 0);

	//Setters



private:

	std::string blockId;
	unsigned int height;
	unsigned int nonce;
	std::string previousBlockId;
	std::string merkleRoot;
	std::vector<Tx> txs;

};


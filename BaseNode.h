#pragma once


#include "Block.h"
#include <vector>
#include <iostream>

class BaseNode
{

public:

	virtual void connect(void) = 0;
	virtual void disconnect(void) = 0;
	virtual void commSend(void) = 0;
	virtual void commReceive(void) = 0;

	virtual void createBlockchainFromFile(void) = 0;

	virtual void getBlock(std::string id) = 0;
	virtual void getBlocksID(int numOfBlocks = 0, int offset = 0) = 0;
	virtual void getBlockQuant(void) = 0;

private:

	std::vector <Block> blockchain;

};


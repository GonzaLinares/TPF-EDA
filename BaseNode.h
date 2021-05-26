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

	virtual void createBlockchainFromFile(std::string&) = 0;
	virtual void deleteBlockchain() = 0;

	virtual Block * getBlock(std::string id) = 0;
	virtual bool getBlocksID(std::vector<std::string>& buffer, int numOfBlocks = 0, int offset = 0) = 0;
	virtual int getBlockQuant(void) = 0;

	
protected:

	std::vector <Block> blockchain;
};


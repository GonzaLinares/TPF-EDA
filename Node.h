#pragma once

/******************************************************************************
* INCLUDE HEADER FILES
******************************************************************************/
#include "Block.h"
#include "BaseNode.h"
#include <vector>

/******************************************************************************
* CLASS NODE
******************************************************************************/
class Node : public BaseNode
{

public:

	// Inherited via BaseNode
	// override specifier ensures that the function is virtual and is overriding a virtual function from a base class
	virtual void connect(void) override;

	virtual void disconnect(void) override;

	virtual void commSend(void) override;

	virtual void commReceive(void) override;

	virtual Block * getBlock(std::string id) override;

	virtual bool getBlocksID(std::vector<std::string>& buffer, int numOfBlocks = 0, int offset = 0) override;

	virtual int getBlockQuant(void) override;

	virtual bool createBlockchainFromFile(std::string&) override;

	virtual void deleteBlockchain() override;


private:
	

};


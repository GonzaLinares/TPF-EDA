#pragma once

#include "Block.h"
#include "BaseNode.h"
#include <vector>

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

	virtual std::vector<std::string> getBlocksID(int numOfBlocks = 0, int offset = 0) override;

	virtual int getBlockQuant(void) override;

	virtual void createBlockchainFromFile(void) override;

private:

};


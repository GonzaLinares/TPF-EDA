#pragma once

#include "Block.h"
#include "BaseNode.h"
#include <vector>

class Node : public BaseNode
{

public:

	// Inherited via BaseNode
	virtual void connect(void) override;

	virtual void disconnect(void) override;

	virtual void commSend(void) override;

	virtual void commReceive(void) override;

	virtual void getBlock(std::string id) override;

	virtual void getBlocksID(int numOfBlocks = 0, int offset = 0) override;

	virtual void getBlockQuant(void) override;

	virtual void createBlockchainFromFile(void);

private:

};


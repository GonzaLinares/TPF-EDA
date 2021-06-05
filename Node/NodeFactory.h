#pragma once

#include "BaseNode.h"

class NodeFactory
{
public:

	std::vector <BaseNode*>& getNodes(void);
	bool createFullNode(void);
	bool createSPVNode(void);


private:

	std::vector <BaseNode*> nodes;
};


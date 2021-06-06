#pragma once

#include "BaseNode.h"

class NodeFactory
{
public:

	NodeFactory(boost::asio::io_context& ioContext_);
	std::vector <BaseNode*>& getNodes(void);
	bool createFullNode(void);
	bool createFullNode(void);
	bool createSPVNode(void);


private:
	boost::asio::io_context ioContext;
	std::vector <BaseNode*> nodes;
};


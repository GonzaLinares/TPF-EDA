#include "NodeFactory.h"
#include "FullNode.h"
#include "SPVNode.h"

NodeFactory::NodeFactory(boost::asio::io_context& ioContext_)
	: ioContext(ioContext_)
{
}

std::vector<BaseNode*>& NodeFactory::getNodes(void)
{
	return nodes;
}

bool NodeFactory::createFullNode(void)
{

	nodes.push_back(new FullNode());

	return true;
}

bool NodeFactory::createSPVNode(void)
{
	nodes.push_back(new SPVNode());

	return true;
}

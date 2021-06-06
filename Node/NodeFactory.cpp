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

bool NodeFactory::createFullNode(std::string port, std::string path2blockchain)
{
	nodes.push_back( new FullNode(ioContext, port, path2blockchain));

	return true; //TODO: Chequear errores de carga
}

bool NodeFactory::createFullNode(std::string port)
{
	nodes.push_back(new FullNode(ioContext, port));

	return true;
}

bool NodeFactory::createSPVNode(std::string port, std::string path2blockchain)
{
	nodes.push_back(new SPVNode(ioContext, port, path2blockchain));

	return true;
}

bool NodeFactory::createSPVNode(std::string port)
{
	nodes.push_back(new SPVNode(ioContext, port));

	return true;
}


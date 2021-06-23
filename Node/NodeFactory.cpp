#include "NodeFactory.h"
#include "FullNode.h"
#include "SPVNode.h"
#include "MinerNode.h"

NodeFactory::NodeFactory(boost::asio::io_context& ioContext_)
	: ioContext(ioContext_)
{
}

NodeFactory::~NodeFactory()
{
	for (auto it : nodes)
	{
		delete it;
	}
	nodes.clear();
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

bool NodeFactory::createMinerNode(std::string port, std::string path2blockchain)
{
	nodes.push_back(new MinerNode(ioContext, port, path2blockchain));

	return true;
}

bool NodeFactory::createMinerNode(std::string port)
{
	nodes.push_back(new MinerNode(ioContext, port));

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

bool NodeFactory::deleteNode(unsigned int nodeIndex)
{
	if (nodes.size() > nodeIndex)
	{
		auto it = nodes.begin();
		it += nodeIndex;
		delete *it;
		nodes.erase(it);
		return true;
	}

	return false;
}

bool NodeFactory::alreadyExist(std::string portNum)
{
	for (auto it : nodes)
	{
		if (it->getPort() == portNum)
		{
			return true;
		}
	}

	return false;
}



bool NodeFactory::pollAllNodes()
{

	ioContext.poll();
	for (auto it : nodes)
	{ 
		it->poll();
	}


	return true;		//TODO: chequear devolucion
}


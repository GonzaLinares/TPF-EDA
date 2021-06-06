#pragma once

#include "BaseNode.h"


class NodeFactory
{
public:

	NodeFactory(boost::asio::io_context& ioContext_);
	std::vector <BaseNode*>& getNodes(void);

	bool createFullNode(boost::asio::io_context& ioContext, std::string port, std::string path2blockchain);
	bool createFullNode(boost::asio::io_context& ioContext, std::string port);

	bool createSPVNode(boost::asio::io_context& ioContext, std::string port, std::string path2blockchain);
	bool createSPVNode(boost::asio::io_context& ioContext, std::string port);

private:
	boost::asio::io_context& ioContext;
	std::vector <BaseNode*> nodes;
};


#pragma once

#include "BaseNode.h"
#include "FullNode.h"

class MinerNode : public FullNode {

public:

	MinerNode(boost::asio::io_context& ioContext, std::string port, std::string path2blockchain);
	MinerNode(boost::asio::io_context& ioContext, std::string port);

	std::string getNodeType();

private:

};


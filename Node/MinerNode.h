#pragma once

#include "BaseNode.h"
#include "FullNode.h"

class MinerNode : public FullNode {

public:

	MinerNode(boost::asio::io_context& ioContext, std::string port, std::string path2blockchain);
	MinerNode(boost::asio::io_context& ioContext, std::string port);

	virtual std::string getNodeType() override;

	virtual bool poll() override;

private:
    
    std::vector<Tx> tx2add;
    Block block2mine;

};


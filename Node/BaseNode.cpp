#include "BaseNode.h"


BaseNode::BaseNode(boost::asio::io_context& ioContext, boost::function<std::string(std::string, std::string)> msgReceivedCb, int portNum)
	: server(ioContext, msgReceivedCb, portNum),
	state(0)
{

}

void BaseNode::commSend(std::string host, std::string path, std::string& msg)
{
}

void BaseNode::commSend(std::string host, std::string path)
{
}

void BaseNode::commReceive(void)
{
}

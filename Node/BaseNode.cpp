#include "BaseNode.h"


void BaseNode::deleteBlockchain()
{
    blockchain.clear();
}

Block* BaseNode::getBlock(std::string id)
{
    Block* salida = nullptr;
    for (std::vector<Block>::iterator it = blockchain.begin(); it != blockchain.end(); it++) {

        if ((it->getId()) == id) {

            salida = &(*it);
        }
    }

    return salida;
}

bool BaseNode::getBlocksID(std::vector<std::string>& buffer, int numOfBlocks, int offset)
{
    bool result = true;
    if (blockchain.size() == 0)
    {
        return false;
    }

    if ((int)blockchain.size() - offset < numOfBlocks)
    {
        numOfBlocks = blockchain.size() - offset;
        result = false;
    }

    std::vector<Block>::reverse_iterator it = blockchain.rbegin();

    if (numOfBlocks == 0 && offset == 0)
    {
        for (int i = 1; i <= (int)blockchain.size(); it++, i++) {

            buffer.push_back(it->getId());
        }
    }
    else
    {
        it += offset;

        for (int i = 1; i <= numOfBlocks; it++, i++) {

            buffer.push_back(it->getId());
        }
    }

    return result;
}

int BaseNode::getBlockQuant(void)
{
    return blockchain.size();
}

BaseNode::BaseNode(boost::asio::io_context& ioContext, boost::function<std::string(std::string, std::string)> msgReceivedCb, int portNum)
	: server(ioContext, msgReceivedCb, portNum),
	state(0),
    client(msgReceivedCb)
{

}

void BaseNode::commSend(std::string host, std::string path, std::string& msg)
{
}

void BaseNode::commSend(std::string host, std::string path)
{
}

std::string BaseNode::getIP()
{
    return server.getLocalEndpointIP();
}

std::string BaseNode::getPort()
{
    return server.getLocalEndpointPort();
}

#include "MinerNode.h"
#include <chrono>

MinerNode::MinerNode(boost::asio::io_context& ioContext, std::string port, std::string path2blockchain)
    : FullNode(ioContext, port, path2blockchain)
{
    if (path2blockchain.size() != 0)
    {
        this->createBlockchainFromFile(path2blockchain);
    }
    mining = false;
}

MinerNode::MinerNode(boost::asio::io_context& ioContext, std::string port)
    : FullNode(ioContext, port) 
{
    mining = false;
}


std::string MinerNode::getNodeType() 
{
    return std::string("Miner");
}


bool MinerNode::poll()
{
    if (!mining)
    {
        prepareBlock2mine();
    }
    else
    {
        if (mine())
        {

        }
    }

    return FullNode::poll();
}

void MinerNode::prepareBlock2mine()
{


    block2mine.setHeight(blockchain.size() + 1);

}

bool MinerNode::mine()
{
    return false;
}

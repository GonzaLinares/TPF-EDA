#include "MinerNode.h"

MinerNode::MinerNode(boost::asio::io_context& ioContext, std::string port, std::string path2blockchain)
    : FullNode(ioContext, port, path2blockchain)
{
    if (path2blockchain.size() != 0)
    {
        this->createBlockchainFromFile(path2blockchain);
    }
}

MinerNode::MinerNode(boost::asio::io_context& ioContext, std::string port)
    : FullNode(ioContext, port) 
{
}
 
std::string MinerNode::getNodeType() {
    return std::string("Miner");
}
bool MinerNode::poll()
{
    return false;
}

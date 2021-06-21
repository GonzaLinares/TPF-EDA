#include "BaseNode.h"


BaseNode::BaseNode(boost::asio::io_context& ioContext, boost::function<std::string(std::string, std::string)> msgReceivedCb, int portNum)
    : server(ioContext, msgReceivedCb, portNum),
    state(IDLE),
    client(msgReceivedCb, portNum + 1)
{
    currentBlock = 0;
}

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

std::string BaseNode::getLastReceivedClient() {
    return lastReClient;
}

int BaseNode::getBlockQuant(void)
{
    return blockchain.size();
}


void BaseNode::commSend(std::string host, std::string path, std::string& msg)
{
    if (host.size() != 0 && msg.size() != 0)
    {
        client.POST(host, path, msg);
        state = SENDING;
    }

}

void BaseNode::commSend(std::string host, std::string path)
{
    if (host.size() != 0)
    {
        client.GET(host, path);
        state = SENDING;
    }
}

std::string BaseNode::getIP()
{
    return server.getLocalEndpointIP();
}

std::string BaseNode::getPort()
{
    return server.getLocalEndpointPort();
}

node_state_t BaseNode::getState()
{
    return state;
}


bool BaseNode::addNeighbour(std::string ipAndPort, std::string nodeType)
{
    if ((this->getNodeType() == "SPV" && nodeType == "SPV")){
        return false;
    }
    else {
        neighbours.push_back(std::pair<std::string, std::string>(ipAndPort, nodeType));
    }
    return true;
}

bool BaseNode::deleteNeighbour(int indexNum)
{
    if (indexNum < neighbours.size())
    {
        auto it = neighbours.begin();
        it += indexNum;
        neighbours.erase(it);
        return true;
    }
    return false;
}

std::vector <std::pair<std::string, std::string>>& BaseNode::getNeighbours() {
    return neighbours;
}

bool BaseNode::hasNeighbours() {
    if (neighbours.empty()) {
        return false;
    }
    else {
        return true;
    }
}

bool BaseNode::poll()
{
    client.poll();

    return true;        //todo: Chequear devolucion
}


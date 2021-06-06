#include "SPVNode.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

std::vector<std::string> SPVNode::actionsVector{"TransactionPost" , "FilterPost" , "GetBlockPost"};

SPVNode::SPVNode(boost::asio::io_context& ioContext, std::string path2blockchain, std::string port)
    : BaseNode(ioContext, boost::bind(&SPVNode::receivedMsgCB, this, boost::placeholders::_1, boost::placeholders::_2), stoi(port))
{

}

SPVNode::SPVNode(boost::asio::io_context& ioContext, std::string port)
    : BaseNode(ioContext, boost::bind(&SPVNode::receivedMsgCB, this, boost::placeholders::_1, boost::placeholders::_2), stoi(port))
{
}

std::string SPVNode::getNodeType()
{
    return std::string("SPVNode");
}

bool SPVNode::addNeighbour(std::string ipAndPort, std::string nodeType)
{
    neighbours.insert(std::pair<std::string, std::string>(ipAndPort, nodeType));

    return false;
}

bool SPVNode::createBlockchainFromFile(std::string& path)
{

    if (path.size() > 0)
    {
        std::ifstream file(path);

        if (file.is_open())
        {
            json jsonFile;
            try
            {
                file >> jsonFile;
                if (jsonFile.is_array() && jsonFile[0].is_object())
                {
                    for (int i = jsonFile.size() - 1; i >= 0; i--)
                    {
                        this->blockchain.push_back(Block(jsonFile[i]["blockid"], jsonFile[i]["height"], jsonFile[i]["merkleroot"], jsonFile[i]["nTx"], jsonFile[i]["nonce"], jsonFile[i]["previousblockid"]));
                    }
                }
                return true;
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << std::endl;
                return false;
            }

        }
    }
    return false;
}

std::vector<std::string> SPVNode::getActionList()
{
    return std::vector<std::string>();
}

bool SPVNode::transactionPost(std::string publicKey, int amount, std::string host)
{
    char buf[50];
    std::string answer;

    answer += std::string(" ""tx"": [ \n ");
    answer += std::string(" {\n");

    //VIN
    answer += std::string(" ""vin"": [ \n ");
    //Aca guardaria las vin
    answer += std::string(" ],\n");

    //VOUT
    answer += std::string(" ""vout"": [ \n ");
    answer += std::string(" {\n");
    answer += std::string(" ""amount"": ");

    sprintf_s(buf, "%d", amount);
    answer += std::string(buf);

    answer += std::string(",\n");
    answer += std::string(" ""publicid"": ");
    answer += std::string("""") + publicKey + std::string(""",\n");
    answer += std::string(" },\n");
    answer += std::string("],\n");

    commSend(host, std::string("eda_coin/send_tx/"), answer);

    return false;

    return false;
}

bool SPVNode::filterPost(std::string host)
{
    std::string answer;

    answer = std::string("{\n ""Key"": ""DamiValue""\n }");

    commSend(host, "eda_coin/send_filter/", answer);

    return false;
}

bool SPVNode::getBlockHeader(std::string blockId, std::string blockCount, std::string host)
{
    char buf[50];

    sprintf_s(buf, "eda_coin/get_block_header?block_id=%s&count=%s", blockId.c_str(), blockCount.c_str());
    commSend(host, std::string(buf));

    return false;
}

std::string SPVNode::merkleBlockPostReceived(bool error, int result)
{
    std::string answer;

    if (error == true) {

        if (result == 1) {

            answer = std::string("{ ""status"": true,\n ""result"": 1 }");
        }
        else {

            answer = std::string("{ ""status"": true,\n ""result"": 2 }");
        }
    }
    else {

        answer = std::string("{ ""status"": true,\n ""result"": null }");
    }

    return answer;
}

std::string SPVNode::receivedMsgCB(std::string client, std::string msg)
{
    if (msg != std::string("")) {

        //Aca podria por ejemplo hacer algo con el mensaje que nos enviaron
    }
    return merkleBlockPostReceived(false, 0);

    //El unico caso en el que llegan mensajes es para la recepción del MerkleBlock
}

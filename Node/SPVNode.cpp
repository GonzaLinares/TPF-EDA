#include "SPVNode.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include "../Hashing.h"

using json = nlohmann::json;

std::vector<std::string> SPVNode::actionsVector{"FilterPost" , "GetBlockHeaders"};

SPVNode::SPVNode(boost::asio::io_context& ioContext, std::string port, std::string path2blockchain)
    : BaseNode(ioContext, boost::bind(&SPVNode::receivedMsgCB, this, boost::placeholders::_1, boost::placeholders::_2), stoi(port))
{
    if (path2blockchain.size() != 0)
    {
        this->createBlockchainFromFile(path2blockchain);
    }
}

SPVNode::SPVNode(boost::asio::io_context& ioContext, std::string port)
    : BaseNode(ioContext, boost::bind(&SPVNode::receivedMsgCB, this, boost::placeholders::_1, boost::placeholders::_2), stoi(port))
{
}

std::string SPVNode::getNodeType()
{
    return std::string("SPV");
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
    return actionsVector;
}

bool SPVNode::transactionPost(std::string publicKey, int amount, std::string host)
{
    char buf[50];
    std::string answer;

    answer += std::string("{ \"tx\": \n ");
    answer += std::string(" {\n");

    //VIN
    answer += std::string(" \"vin\": [ \n ");
    //Aca guardaria las vin
    answer += std::string(" ],\n");

    //VOUT
    answer += std::string(" \"vout\": [ \n ");
    answer += std::string(" {\n");
    answer += std::string(" \"amount\": ");

    sprintf_s(buf, "%d", amount);
    answer += std::string(buf);

    answer += std::string(",\n");
    answer += std::string(" \"publicid\": ");
    answer += std::string("\"") + publicKey + std::string("\",\n");
    answer += std::string("}\n");
    answer += std::string("]\n");
    answer += std::string("}\n");
    answer += std::string("}\n");

    commSend(host, std::string("eda_coin/send_tx/"), answer);

    return false;

}

bool SPVNode::filterPost(std::string host)
{
    std::string answer;

    answer = std::string("{\n \"Key\": \"DamiValue\"\n }");

    commSend(host, "eda_coin/send_filter/", answer);

    return false;
}

bool SPVNode::getBlockHeader(std::string blockId, std::string blockCount, std::string host)         //TODO: Dami no te hagas el vivo y pedi un numero en block count
{
    char buf[100];

    sprintf_s(buf, "eda_coin/get_block_header/?block_id=%s&count=%s", blockId.c_str(), blockCount.c_str());
    commSend(host, std::string(buf));

    return false;
}

std::string SPVNode::merkleBlockPostReceived(bool error, int result)
{
    std::string answer;

    if (error == true) {

        if (result == 1) {

            answer = std::string("{ \"status\": true,\n \"result\": 1 }");
        }
        else {

            answer = std::string("{ \"status\": true,\n \"result\": 2 }");
        }
    }
    else {

        answer = std::string("{ \"status\": true,\n \"result\": null }");
    }

    return answer;
}

std::string SPVNode::receivedMsgCB(std::string client, std::string msg)
{
    std::string answer;
    std::string host = client.substr(0, client.find_first_of('/', 0));

    lastReClient = host;
    client.erase(0, client.find_first_of('/', 0));

    bool error = false; //Parametros que serán enviados a las funciones de validación
    int result = 0;

    if (client == std::string("/eda_coin/send_merkle_block/")) {

        if (msg != std::string("")) {

            validateMerkleBlockPost(error, result, msg);
            answer = merkleBlockPostReceived(error, result);
        }
    }
    else
    {
#ifdef DEBUGCALLBACK
        std::cout << "Recieved at:" << getIP() + ":" + std::to_string(std::stoi(getPort()) + 1) << std::endl;
        std::cout << "*************************" << std::endl;
        std::cout << msg << std::endl;
        std::cout << "*************************" << std::endl << std::endl;
#endif // DEBUGCALLBACK
    }

    return answer;
    //El unico caso en el que llegan mensajes es para la recepción del MerkleBlock
}

void SPVNode::validateMerkleBlockPost(bool& error, int& result, std::string msg)
{
    json jsonFile;

    std::string blockid = jsonFile["blockid"];
    unsigned int height = jsonFile["height"];
    std::string merkleroot = jsonFile["merkleroot"];
    unsigned int nTx = jsonFile["nTx"];
    int nonce = jsonFile["nonce"];
    std::string previousblockid = jsonFile["previousblockid"];

    Block blockSent(blockid, height, merkleroot, nTx, nonce, previousblockid);

    //Verificar que cumple con el challenge.
    std::string tempString;
    tempString = previousblockid + merkleroot + hexCodedAscii(nonce);
    std::string tempString1 = hash32(tempString);
    tempString = hash32(tempString1);

    if (tempString != blockid) {

        error = true;
        result = 2;
        return;
    }

    tempString = blockid.substr(0, NUMBEROFZEROSCHALLENGE - 1);
    for (int i = 0; i < 10; i++) {

        if (tempString[i] != '0') {
            error = true;
            result = 2;
            return;
        }
    }

    //El previous block hash coincide con el block hash del bloque anterior
    if (previousblockid == "0000000000000000000000000000000000000000000000000000000000000000" && currentBlock != 0) {

        error = true;
        result = 2;
        return;
    }
    else if (previousblockid != (blockchain[currentBlock]).getId()) {

        error = true;
        result = 2;
        return;
    }

    blockchain.push_back(blockSent);
}

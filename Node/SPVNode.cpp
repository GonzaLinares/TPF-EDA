#include "SPVNode.h"
#include <iostream>

std::vector<std::string> SPVNode::actionsVector{ "ACA VAN LOS NOMBRES DE LAS ACCIONES" };

bool SPVNode::createBlockchainFromFile(std::string&)
{
    return false;
}

void SPVNode::deleteBlockchain()
{
}

Block* SPVNode::getBlock(std::string id)
{
    return nullptr;
}

bool SPVNode::getBlocksID(std::vector<std::string>& buffer, int numOfBlocks, int offset)
{
    return false;
}

int SPVNode::getBlockQuant(void)
{
    return 0;
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

bool SPVNode::merkleBlockPostReceived(bool error, int result, std::string host)
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

    commSend(host, "QUE,PATH,VA,?", answer);

    return false;
}

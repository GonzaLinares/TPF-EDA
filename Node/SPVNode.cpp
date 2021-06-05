#include "SPVNode.h"

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

bool SPVNode::transactionPost(std::string blockId, std::string host)
{
    return false;
}

bool SPVNode::filterPost()
{
    return false;
}

bool SPVNode::getBlockHeader(std::string blockId, std::string blockCount, std::string host)
{
    char buf[50];

    sprintf_s(buf, "eda_coin/get_block_header?block_id=%s&count=%s", blockId.c_str(), blockCount.c_str());

    commSend(host, std::string(buf));

    return false;
}

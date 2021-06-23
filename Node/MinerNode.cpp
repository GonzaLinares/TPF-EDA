#include "MinerNode.h"
#include <chrono>

#define BLOCKS_TILL_UPDATE 6
#define DEFAULT_TARGET 2
#define SECOND_PER_BLOCK 60

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
            blockchain.push_back(block2mine);       //TODO: Que hago con el bloque minado?
            block2mine.clear();
            mining = false;
        }
    }

    return FullNode::poll();
}

void MinerNode::prepareBlock2mine()
{
    unsigned int newTarget;

    block2mine.setHeight(blockchain.size() + 1);

    if (blockchain.size() && ((block2mine.getHeight()-1) % BLOCKS_TILL_UPDATE) == 0)
    {
        float avgTimeperBlock = 0;

        for (int i = 0; i < BLOCKS_TILL_UPDATE-2; i++)
        {
            avgTimeperBlock += blockchain[blockchain.size() - 1 - i].getTimestamp() - blockchain[blockchain.size() - 2 - i].getTimestamp();
        }
        avgTimeperBlock /= 5.0F;
        if (avgTimeperBlock < SECOND_PER_BLOCK - 10)
        {
            newTarget = blockchain[blockchain.size() - 1].getTarget() + 1;
        }
        else
        {
            newTarget = blockchain[blockchain.size() - 1].getTarget() - 1;
        }
    }
    else if (blockchain.size())
    {
        newTarget = blockchain[blockchain.size() - 1].getTarget();
    }
    else
    {
        newTarget = DEFAULT_TARGET;
    }
    block2mine.setTarget(newTarget);
    for (auto it : tx2add)
    {
        block2mine.push_transaction(it);
    }
    OutTx rewardOut(privateKey, blockReward);
    Tx reward;
    reward.push_vout(rewardOut);
    reward.calculateTXID();
    block2mine.push_transaction(reward);
    block2mine.calculateMerkleRoot();
    if (blockchain.size() == 0)
    {
        block2mine.setPrevBlockId("00000000000000000000000000000000");
    }
    else
    {
        block2mine.setPrevBlockId(blockchain[blockchain.size() - 1].getId());
    }
    
    mining = true;

}

bool MinerNode::mine()
{
    
    block2mine.calculateHash();

    for (unsigned int i = 0; i < block2mine.getTarget(); i++)
    {
        if (block2mine.getId()[i] != '0')
        {
            block2mine.setNonce(block2mine.getNonce() + 1);
            return false;
        }
    }
    auto now = std::chrono::system_clock::now();
    block2mine.setTimestamp(std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count());

    return true;
}

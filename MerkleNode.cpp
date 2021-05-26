#include "MerkleNode.h"


MerkleNode::MerkleNode(std::string& data) {

    hash = data;
    right = nullptr;
    left = nullptr;
}

MerkleNode* MerkleNode::Left(void)
{
    return left;
}

MerkleNode* MerkleNode::Right(void)
{
    return right;
}

void MerkleNode::getMerklePath(std::string txid, std::vector<std::string> merklepath)
{

    if (Left() == nullptr || Right() == nullptr)
    {
        return;
    }

    if (Left()->hash == txid || Right()->hash == txid)
    {
        if (left->hash == txid)
        {
            merklepath.push_back(Right()->hash);
            return;
        }
        else
        {
            merklepath.push_back(Left()->hash);
            return;
        }
    }
    else
    {
        Left()->getMerklePath(txid, merklepath);
        if (Left()->Right()->hash == *merklepath.end() || Left()->Left()->hash == *merklepath.end())
        {
            merklepath.push_back(Right()->hash);
            return;
        }
        right->getMerklePath(txid, merklepath);
        if (Right()->Right()->hash == *merklepath.end() || Right()->Left()->hash == *merklepath.end())
        {
            merklepath.push_back(Left()->hash);
            return;
        }
    }

    return;
}

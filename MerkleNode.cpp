#include "MerkleNode.h"



MerkleNode::MerkleNode(std::string data) {

    hash = data;
    right = nullptr;
    left = nullptr;
}

MerkleNode::~MerkleNode()
{
    if (left != nullptr)
    {
        delete left;
    }
    if (right != nullptr)
    {
        delete right;
    }

}

std::string MerkleNode::getHash()
{
    return hash;
}

MerkleNode*& MerkleNode::Left(void)
{
    return left;
}

MerkleNode*& MerkleNode::Right(void)
{
    return right;
}

void MerkleNode::getMerklePath(std::string& txid, std::vector<std::string>& merklepath)
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
        if (merklepath.size() > 0 && (Left()->Right()->hash == merklepath.back() || Left()->Left()->hash == merklepath.back()))
        {
            merklepath.push_back(Right()->hash);
            return;
        }
        right->getMerklePath(txid, merklepath);
        if (merklepath.size() > 0 && (Right()->Right()->hash == merklepath.back() || Right()->Left()->hash == merklepath.back()))
        {
            merklepath.push_back(Left()->hash);
            return;
        }
    }

    return;
}

void MerkleNode::printNode(int indent) {
        if (left) {
            left->printNode(indent + 4);
        }
        if (n->right) {
            right->printNode(indent + 4);
        }
        if (indent) {
            std::cout << std::setw(indent) << ' ';
        }
        std::cout << n->hash[0] << "\n ";
}

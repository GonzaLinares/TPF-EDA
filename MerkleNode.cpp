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

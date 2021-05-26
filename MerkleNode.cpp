#include "MerkleNode.h"

MerkleNode::MerkleNode(std::string data) {

    hash = data;
    right = nullptr;
    left = nullptr;
}
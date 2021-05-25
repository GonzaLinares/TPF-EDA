#pragma once

#include <iostream>

struct MerkleNode {

    std::string hash;
    MerkleNode* left;
    MerkleNode* right;

    MerkleNode(std::string data);
};
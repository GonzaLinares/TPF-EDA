#pragma once

#include <iostream>

class MerkleNode {

    public:
    std::string hash;
    MerkleNode* left;
    MerkleNode* right;

    MerkleNode(std::string& data);
    MerkleNode* Left(void);
    MerkleNode* Right(void);
};
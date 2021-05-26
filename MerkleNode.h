#pragma once

#include <iostream>
#include <vector>

class MerkleNode {

    public:             //TODO: privatizar
    std::string hash;
    MerkleNode* left;
    MerkleNode* right;

    MerkleNode(std::string& data);
    MerkleNode* Left(void);
    MerkleNode* Right(void);

    void getMerklePath(std::string& txid, std::vector<std::string>& merklepath);
};

#pragma once

#include <iostream>
#include <vector>

class MerkleNode {

    public:             //TODO: privatizar


    MerkleNode(std::string data);
    ~MerkleNode();
    std::string getHash();
    MerkleNode*& Left(void);
    MerkleNode*& Right(void);

    void getMerklePath(std::string& txid, std::vector<std::string>& merklepath);

    void printNode(int indent);

private:
    std::string hash;
    MerkleNode* left;
    MerkleNode* right;
};

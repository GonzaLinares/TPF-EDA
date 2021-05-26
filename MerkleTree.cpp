#include "MerkleTree.h"


template<std::string(*hashFunc)(std::string&)>
inline MerkleTree<hashFunc>::MerkleTree(std::vector<std::string> txIds)
{
    std::vector<MerkleNode*> nodes;
    MerkleNode tempNode;
    tempNode.left = nullptr;
    tempNode.right = nullptr;
    std::string tempString;

    while (txIds.size() != 1) {

        for (unsigned int l = 0, n = 0; l < txIds.size(); l = l + 2, n++) {

            if (l != txIds.size() - 1) {

                tempString = txIds[l] + txIds[l + 1];
                tempNode.hash = (*hashFunc(tempString.c_str()));
                nodes.push_back(tempNode);
                nodes[n]->left = txIds[l];
                nodes[n]->right = txIds[l + 1];
            }
            else {
                nodes.push_back(txIds[l]);
            }
        }
        std::cout << "\n";
        
        txIds.clear();

        for (std::vector<MerkleNode>::iterator it = nodes.begin(); it != nodes.end(); it++) {

            txIds.push_back(it->hash);
        }

        nodes.clear();
    }
}

template<std::string(*hashFunc)(std::string&)>
std::string MerkleTree<hashFunc>::getMerkleRoot(void)
{
    return (root->hash);
}

template<std::string(*hashFunc)(std::string&)>
void MerkleTree<hashFunc>::getMerklePath(std::string txid, std::vector<std::string> merklepath)
{



}

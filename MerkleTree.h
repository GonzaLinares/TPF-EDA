#pragma once

#include <iostream>
#include "Tx.h"
#include "MerkleNode.h"

//TODO: arreglar la prolijidad

template <std::string (*hashFunc)(std::string&)> class MerkleTree
{
	
public:

	MerkleTree(std::vector <std::string>& txIds);
	std::string getMerkleRoot(void);
	void getMerklePath(std::string& txid, std::vector<std::string>& merklepath);

private:

	MerkleNode* root;
	std::vector<MerkleNode*> blocks;

};
template<std::string(*hashFunc)(std::string&)> MerkleTree<hashFunc>::MerkleTree(std::vector<std::string>& txIds)
{
    /*Al vector Blocks te lo meti adentro de la clase, sino se moriria todo al cerrar la funcion creo*/

    std::vector<MerkleNode*> nodes;

    for (auto it : txIds) {

        blocks.push_back(new MerkleNode(it));
    }

    while (blocks.size() != 1) {

        for (unsigned int l = 0, n = 0; l < blocks.size(); l = l + 2, n++) {
            if (l != blocks.size() - 1) {
                std::string aux = blocks[l]->hash + blocks[l + 1]->hash;
                aux = hashFunc(aux);
                nodes.push_back(new MerkleNode(aux));
                nodes[n]->left = blocks[l];
                nodes[n]->right = blocks[l + 1];
            }
            else {
                nodes.push_back(blocks[l]);
            }
        }

        blocks = nodes;
        nodes.clear();
    }

    root = blocks[0];
}

template<std::string(*hashFunc)(std::string&)>
std::string MerkleTree<hashFunc>::getMerkleRoot(void)
{
    return (root->hash);
}

template<std::string(*hashFunc)(std::string&)>
void MerkleTree<hashFunc>::getMerklePath(std::string& txid, std::vector<std::string>& merklepath)
{

    root->getMerklePath(txid, merklepath);

}
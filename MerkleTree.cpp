#include "MerkleTree.h"
#include "Hashing.h"

template class MerkleTree<hash32>;

static void deleteTree(MerkleNode* n);

template<std::string(*hashFunc)(std::string&)> MerkleTree<hashFunc>::MerkleTree()
{
    root = nullptr;
}

template<std::string(*hashFunc)(std::string&)> MerkleTree<hashFunc>::MerkleTree(std::vector<std::string>& txIds)
{
    /*Al vector Blocks te lo meti adentro de la clase, sino se moriria todo al cerrar la funcion creo*/

    std::vector<MerkleNode*> nodes;

    for (auto it : txIds) {

        blocks.push_back(new MerkleNode(it));
    }

    while (blocks.size() != 1) {

        for (unsigned int i = 0, n = 0; i < blocks.size(); i = i + 2, n++) {
            if (blocks.size() % 2 != 0) {
                blocks.push_back(new MerkleNode(blocks.back()->getHash()));
            }
            std::string aux = blocks[i]->getHash() + blocks[i + 1]->getHash();
            aux = hashFunc(aux);
            nodes.push_back(new MerkleNode(aux));
            nodes[n]->Left() = blocks[i];
            nodes[n]->Right() = blocks[i + 1];
        }

        blocks = nodes;
        nodes.clear();
    }

    root = blocks[0];
}

template<std::string(*hashFunc)(std::string&)>
MerkleTree<hashFunc>::~MerkleTree()
{
    if (root != nullptr)
    {
        deleteTree(root);
    }
    root = nullptr;
}

template<std::string(*hashFunc)(std::string&)>
std::string MerkleTree<hashFunc>::getMerkleRoot(void)
{
    if (root != nullptr) {
        return (root->getHash());
    }

    return "";
}

template<std::string(*hashFunc)(std::string&)>
MerkleNode* MerkleTree<hashFunc>::getRootNode(void)
{
    return root;
}

template<std::string(*hashFunc)(std::string&)>
void MerkleTree<hashFunc>::getMerklePath(std::string& txid, std::vector<std::string>& merklepath)
{

    root->getMerklePath(txid, merklepath);

}

void deleteTree(MerkleNode* n) {

    if (n) {
        deleteTree(n->Left());
        deleteTree(n->Right());
        delete n;
    }
}
#include "MerkleTree.h"


template<std::string(*hashFunc)(std::string&)>
inline MerkleTree<hashFunc>::MerkleTree(std::vector<std::string> txIds)
{
    /*Al vector Blocks te lo meti adentro de la clase, sino se moriria todo al cerrar la funcion creo*/

    std::vector<MerkleNode*> nodes;

    for (std::vector<std::string>::iterator it : txIds) {

        blocks.push_back(new MerkleNode(*it));  
    }

    while (blocks.size() != 1) {

        for (unsigned int l = 0, n = 0; l < blocks.size(); l = l + 2, n++) {
            if (l != blocks.size() - 1) { 
                nodes.push_back(new MerkleNode(hashFunc(blocks[l]->hash + blocks[l + 1]->hash))); 
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
void MerkleTree<hashFunc>::getMerklePath(std::string txid, std::vector<std::string> merklepath)
{



}

//Pseudo codigo para obtener merklepath
/*
string func(txid, buffer){

    if (node.left or right == null)
    {
        return;
    }

    if (node.left == txid or node.right == txid)
    {
        if (node.left == txid)
        {
            buffer.push(node.right.data);
            return;
        }
        else
        {
            buffer.push(node.left.data);
            return;
        }
        
    }
    else
    {
        node.left.func(txid, buffer);
        if (node.left.right.data == buffer[0] or node.left.left.data == buffer[0])
        {
            buffer.push(node.right.data);
            return;
        }
        node.right.func(txid, buffer);
        if (node.right.right.data == buffer[0] or node.right.left.data == buffer[0])
        {
            buffer.push(node.left.data);
            return;
        }
    }
    return

}*/
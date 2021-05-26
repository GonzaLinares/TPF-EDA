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
                tempNode.hash = hashFunc(tempString);
                nodes.push_back(tempNode);
                nodes[n].left = txIds[l];
                nodes[n].right = txIds[l + 1];
            }
            else {
                nodes.push_back(txIds[l]);
            }
        }
        std::cout << "\n";
        
        txIds.clear();

        for (std::vector<MerkleNode>::iterator it : nodes) {

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
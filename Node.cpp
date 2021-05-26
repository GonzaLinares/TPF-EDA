#include "Node.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void Node::connect(void)
{
}

void Node::disconnect(void)
{
}

void Node::commSend(void)
{
}

void Node::commReceive(void)
{
}

//Devuelve un puntero al bloque del id pedido, en el caso de no encontrar ninguno, devuelve NULL
Block* Node::getBlock(std::string id)
{
    Block* salida = nullptr;
    for (std::vector<Block>::iterator it = blockchain.begin(); it != blockchain.end(); it++) {

        if ((it->getId()) == id) {

            salida = &(*it);
        }
    }

    return salida;
}

bool Node::getBlocksID(std::vector<std::string>& buffer, int numOfBlocks, int offset)	//TODO: Que verga pasaba si numOfBlocks es 0 
{
    if (blockchain.size() == 0)
    {
        return false;
    }

    if (blockchain.size() - offset < numOfBlocks)
    {
        numOfBlocks = blockchain.size() - offset;
    }

    std::vector<Block>::reverse_iterator it = blockchain.rbegin();

    if (numOfBlocks == 0 && offset == 0)
    {
        for (int i = 1; i <= blockchain.size(); it++, i++) {

            buffer.push_back(it->getId());
        }
    }
    else
    {
        it += offset;

        for (int i = 1; i <= numOfBlocks ; it++, i++) {

            buffer.push_back(it->getId());
        }
    }
    

    return true;
}

int Node::getBlockQuant(void)
{
    return blockchain.size();
}

void Node::createBlockchainFromFile(std::string& path)
{
    if (path.size() > 0)
    {
        std::ifstream file(path);		//TODO: Chequear por posibles excepciones si path es cualquier verdura
        if (file.is_open())
        {
            json jsonFile;
            file >> jsonFile;
            try
            {
                if (jsonFile.is_array() && jsonFile[0].is_object())
                    {
                        for (int i = jsonFile.size() - 1; i >= 0; i--)
                        {
                            this->blockchain.push_back(Block(jsonFile[i]["blockid"], jsonFile[i]["height"], jsonFile[i]["merkleroot"], jsonFile[i]["nTx"], jsonFile[i]["nonce"], jsonFile[i]["previousblockid"]));
                            for (int j = jsonFile[i]["nTx"] - 1; j >= 0; j--)
                            {
                                Tx auxTx(jsonFile[i]["tx"][j]["txid"], jsonFile[i]["tx"][j]["nTxin"], jsonFile[i]["tx"][j]["nTxout"]);
                                for (int k = jsonFile[i]["tx"][j]["nTxin"] - 1; k >= 0; k--)
                                {
                                    InTx auxInTx(jsonFile[i]["tx"][j]["vin"][k]["blockid"], jsonFile[i]["tx"][j]["vin"][k]["txid"], jsonFile[i]["tx"][j]["vin"][k]["signature"], jsonFile[i]["tx"][j]["vin"][k]["outputIndex"]);
                                    auxTx.push_vin(auxInTx);

                                }

                                for (int k = jsonFile[i]["tx"][j]["nTxout"] - 1; k >= 0; k--)
                                {
                                    OutTx auxOutTx(jsonFile[i]["tx"][j]["vout"][k]["publicid"], jsonFile[i]["tx"][j]["vout"][k]["amount"]);
                                    auxTx.push_vout(auxOutTx);
                                }
                                blockchain[jsonFile.size() - 1 - i].push_transaction(auxTx);
                            }
                            

                            
                        }
                    }
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << std::endl;
            }
            
        }
    }
}

void Node::deleteBlockchain()
{
    blockchain.clear();
}

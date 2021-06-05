#include "FullNode.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


std::vector<std::string> FullNode::actionsVector{ "ACA VAN LOS NOMBRES DE LAS ACCIONES" };

void FullNode::commSend(void)
{

}

void FullNode::commReceive(void)
{
}

//Devuelve un puntero al bloque del id pedido, en el caso de no encontrar ninguno, devuelve NULL
Block* FullNode::getBlock(std::string id)
{
    Block* salida = nullptr;
    for (std::vector<Block>::iterator it = blockchain.begin(); it != blockchain.end(); it++) {

        if ((it->getId()) == id) {

            salida = &(*it);
        }
    }

    return salida;
}

bool FullNode::getBlocksID(std::vector<std::string>& buffer, int numOfBlocks, int offset)
{
    bool result = true;
    if (blockchain.size() == 0)
    {
        return false;
    }

    if ((int)blockchain.size() - offset < numOfBlocks)
    {
        numOfBlocks = blockchain.size() - offset;
        result = false;
    }

    std::vector<Block>::reverse_iterator it = blockchain.rbegin();

    if (numOfBlocks == 0 && offset == 0)
    {
        for (int i = 1; i <= (int)blockchain.size(); it++, i++) {

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
    

    return result;
}

int FullNode::getBlockQuant(void)
{
    return blockchain.size();
}

bool FullNode::createBlockchainFromFile(std::string& path)
{
    if (path.size() > 0)
    {
        std::ifstream file(path);

        if (file.is_open())
        {
            json jsonFile;
            try
            {
                file >> jsonFile;
                if (jsonFile.is_array() && jsonFile[0].is_object())
                {
                    for (int i = jsonFile.size() - 1; i >= 0; i--)
                    {
                        this->blockchain.push_back(Block(jsonFile[i]["blockid"], jsonFile[i]["height"], jsonFile[i]["merkleroot"], jsonFile[i]["nTx"], jsonFile[i]["nonce"], jsonFile[i]["previousblockid"]));
                        for (int j = jsonFile[i]["nTx"] - 1; j >= 0; j--)
                        {
                            Tx auxTx(jsonFile[i]["tx"][j]["txid"]);
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
                return true;
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << std::endl;
                return false;
            }
            
        }
    }
    return false;
}

void FullNode::deleteBlockchain()
{
    blockchain.clear();
}

std::vector<std::string> FullNode::getActionList()
{
    return actionsVector;
}

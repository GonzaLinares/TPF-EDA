#include "FullNode.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::vector<std::string> FullNode::actionsVector{ "ACA VAN LOS NOMBRES DE LAS ACCIONES" };

void FullNode::commSend(std::string host, std::string path, std::string& msg)
{

}

void FullNode::commSend(std::string host, std::string path)
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
    jsonBlockChain = path;

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

bool FullNode::blockPost(std::string host)
{

    commSend(host, std::string("eda_coin/send_block/"), jsonBlockChain);

    return false;
}

bool FullNode::transactionPost(std::string blockId, std::string host)
{

    return false;
}

bool FullNode::merkleBlockPost(std::string blockId, std::string host)
{


    return false;
}

bool FullNode::getBlocks(std::string blockId, std::string blockCount, std::string host)
{
    char buf[50];

    sprintf_s(buf, "eda_coin/get_blocks?block_id=%s&count=%s", blockId.c_str(), blockCount.c_str());

    commSend(host, std::string(buf));

    return false;
}

bool FullNode::blockPostReceived(bool error, int result, std::string host)
{
    std::string answer;

    if (error == true) {
        
        if (result == 1) {

            answer = std::string("{ ""status"": true,\n ""result"": 1 }");
        }
        else {

            answer = std::string("{ ""status"": true,\n ""result"": 2 }");
        }
    }
    else {

        answer = std::string("{ ""status"": true,\n ""result"": null }");
    }

    commSend(host, "QUE,PATH,VA,?", answer);

    return false;
}

bool FullNode::getBlockHeaderReceived(std::string blockID, int count, std::string host)
{

    std::string answer = std::string("{ ""status"": true,\n ""result"": ");
    bool startCopying = false;
    int i = 0;

    //Busco el bloque que me pidieron

    if (blockID == std::string("0x00000000")) {

        startCopying = true;
    }

    for (std::vector<Block>::iterator it = blockchain.begin(); it != blockchain.end() && i < count; it++) {

        if (startCopying == true) {

            answer += std::string(" { ""blockid"": ");
            answer += std::string("""") + it->getId() + std::string(""",\n");
            answer += std::string(" ""height"": ");
            answer += std::to_string(it->getHeight()) + std::string(",\n");
            answer += std::string(" ""merkleroot"": ");
            answer += std::string("""") + it->getMerkleRoot() + std::string(""",\n");
            answer += std::string(" ""nTx"": ");
            answer += std::to_string(it->getnTx()) + std::string(",\n");
            answer += std::string(" ""nonce"": ");
            answer += std::to_string(it->getNonce()) + std::string(",\n");
            answer += std::string(" ""previousblockid"": ");
            answer += std::string("""") + it->getPrevBlockId() + std::string(""",\n");
            answer += std::string("},");
            i++;
        }
        if ((it->getId()) == blockID ) {

            startCopying = true;
        }
    }

    commSend(host, std::string("cual vergas es el path"), answer);

    return false;
}

bool FullNode::getBlocksReceived(std::string blockID, int count, std::string host)
{
    std::string answer = std::string("{ ""status"": true,\n ""result"": ");
    bool startCopying = false;
    int i = 0;

    //Busco el bloque que me pidieron

    if (blockID == std::string("0x00000000")) {

        startCopying = true;
    }

    for (std::vector<Block>::iterator it = blockchain.begin(); it != blockchain.end() && i < count; it++) {

        if (startCopying == true) {

            answer += std::string(" { ""blockid"": ");
            answer += std::string("""") + it->getId() + std::string(""",\n");
            answer += std::string(" ""height"": ");
            answer += std::to_string(it->getHeight()) + std::string(",\n");
            answer += std::string(" ""merkleroot"": ");
            answer += std::string("""") + it->getMerkleRoot() + std::string(""",\n");
            answer += std::string(" ""nTx"": ");
            answer += std::to_string(it->getnTx()) + std::string(",\n");
            answer += std::string(" ""nonce"": ");
            answer += std::to_string(it->getNonce()) + std::string(",\n");
            answer += std::string(" ""previousblockid"": ");
            answer += std::string("""") + it->getPrevBlockId() + std::string(""",\n");
            answer += std::string(" ""tx"": [ \n ");
            //Hacer un iterador por todas las transacciones e ir construyendo de ahi

            i++;
        }
        if ((it->getId()) == blockID) {

            startCopying = true;
        }
    }

    commSend(host, std::string("cual vergas es el path"), answer);

    return false;
}

std::vector<std::string> FullNode::getActionList()
{
    return actionsVector;
}
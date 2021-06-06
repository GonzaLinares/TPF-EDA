#include "FullNode.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include "..\MerkleTree.h"
#include "..\Hashing.h"

using json = nlohmann::json;

std::vector<std::string> FullNode::actionsVector{ "BlockPost", "TransactionPost", "MerkleBlockPost", "GetBlocksPost" };

FullNode::FullNode(boost::asio::io_context& ioContext, std::string port, std::string path2blockchain)
    : BaseNode(ioContext, boost::bind(&FullNode::receivedMsgCB, this, boost::placeholders::_1, boost::placeholders::_2), stoi(port))
{
    if (path2blockchain.size() != 0)
    {
        this->createBlockchainFromFile(path2blockchain);
    }
}

FullNode::FullNode(boost::asio::io_context& ioContext, std::string port)
    : BaseNode(ioContext, boost::bind(&FullNode::receivedMsgCB, this, boost::placeholders::_1, boost::placeholders::_2), stoi(port))
{

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

std::string FullNode::getNodeType()
{
    return std::string("Full");
}

bool FullNode::addNeighbour(std::string ipAndPort, std::string nodeType)
{
    neighbours.push_back(std::pair<std::string, std::string>(ipAndPort, nodeType));

    return false;
}

bool FullNode::blockPost(std::string host)
{
    std::string answer = std::string("{ ""status"": true,\n ""result"": ");
    std::vector<Tx> tempTx;
    std::vector<InTx> tempInTx;
    std::vector<OutTx> tempOutTx;

    //Busco el bloque que me pidieron

    for (std::vector<Block>::iterator it = blockchain.begin(); it != blockchain.end(); it++) {

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
        tempTx = it->getTxVector();

        for (std::vector<Tx>::iterator ut = tempTx.begin(); ut != tempTx.end(); ut++) {

            answer += std::string(" {\n");
            answer += std::string(" ""nTxin"": ");
            answer += std::to_string(ut->getVin().size()) + std::string(",\n");
            answer += std::string(" ""nTxout"": ");
            answer += std::to_string(ut->getVout().size()) + std::string(",\n");
            answer += std::string("  ""txid"": ");
            answer += std::string("""") + ut->getId() + std::string(""",\n");


            //Imprimo las VIN
            answer += std::string(" ""vin"": [ \n ");
            tempInTx = ut->getVin();
            tempOutTx = ut->getVout();

            for (std::vector<InTx>::iterator at = tempInTx.begin(); at != tempInTx.end(); at++) {

                answer += std::string(" {\n");
                answer += std::string(" ""blockid"": ");
                answer += std::string("""") + at->getBlockId() + std::string(""",\n");
                answer += std::string(" ""outputIndex"": ");
                answer += std::to_string(at->getOutputIndex()) + std::string(",\n");
                answer += std::string(" ""signature"": ");
                answer += std::string("""") + at->getSignature() + std::string(""",\n");
                answer += std::string(" ""txid"": ");
                answer += std::string("""") + at->getTxid() + std::string(""",\n");

                if (at + 1 == tempInTx.end()) {
                    answer += std::string(" }\n");
                }
                else {
                    answer += std::string(" },\n");
                }

            }
            answer += std::string(" ],\n");

            //Imprimo las VOUT
            answer += std::string(" ""vout"": [ \n ");

            for (std::vector<OutTx>::iterator at = tempOutTx.begin(); at != tempOutTx.end(); at++) {

                answer += std::string(" {\n");
                answer += std::string(" ""amount"": ");
                answer += std::to_string(at->getAmount()) + std::string(",\n");
                answer += std::string(" ""publicid"": ");
                answer += std::string("""") + at->getPublicId() + std::string(""",\n");

                if (at + 1 == tempOutTx.end()) {

                    answer += std::string(" }\n");
                }
                else {

                    answer += std::string(" },\n");
                }
            }
            answer += std::string("],\n");
        }
    }

    commSend(host, std::string("eda_coin/send_block/"), answer);

    return false;
}

bool FullNode::transactionPost(std::string publicKey, int amount, std::string host)
{
    std::string answer;

    answer += std::string(" ""tx"": [ \n ");
    answer += std::string(" {\n");

    //VIN
    answer += std::string(" ""vin"": [ \n ");
    //Aca guardaria las vin
    answer += std::string(" ],\n");

    //VOUT
    answer += std::string(" ""vout"": [ \n ");
    answer += std::string(" {\n");
    answer += std::string(" ""amount"": ");
    answer += std::to_string(amount) + std::string(",\n");
    answer += std::string(" ""publicid"": ");
    answer += std::string("""") + publicKey + std::string(""",\n");
    answer += std::string(" },\n");
    answer += std::string("],\n");

    commSend(host, std::string("eda_coin/send_tx/"), answer);

    return false;
}

bool FullNode::merkleBlockPost(std::string blockId, int position, std::string host)
{
    std::string answer;
    bool startCopying = false;
    std::vector<Tx> tempTx;
    std::vector<InTx> tempInTx;
    std::vector<OutTx> tempOutTx;
    std::vector<std::string> merkleTxsIDs;
    std::string txIDchosen;
    int i = 0;
    //Busco el bloque que me pidieron

    for (std::vector<Block>::iterator it = blockchain.begin(); it != blockchain.end(); it++) {

        if ((it->getId()) == blockId) {

            startCopying = true;
        }
        if (startCopying == true) {

            answer += std::string(" { ""blockid"": ");
            answer += std::string("""") + it->getId() + std::string(""",\n");   
            answer += std::string(" ""tx"": { \n ");
            tempTx = it->getTxVector();

            for (std::vector<Tx>::iterator ut = tempTx.begin(); ut != tempTx.end(); ut++) {

                merkleTxsIDs.push_back(ut->getId());

                if (i == position) {

                    txIDchosen = ut->getId();

                    answer += std::string(" {\n");
                    answer += std::string(" ""nTxin"": ");
                    answer += std::to_string(ut->getVin().size()) + std::string(",\n");
                    answer += std::string(" ""nTxout"": ");
                    answer += std::to_string(ut->getVout().size()) + std::string(",\n");
                    answer += std::string("  ""txid"": ");
                    answer += std::string("""") + ut->getId() + std::string(""",\n");


                    //Imprimo las VIN
                    answer += std::string(" ""vin"": [ \n ");
                    tempInTx = ut->getVin();
                    tempOutTx = ut->getVout();

                    for (std::vector<InTx>::iterator at = tempInTx.begin(); at != tempInTx.end(); at++) {

                        answer += std::string(" {\n");
                        answer += std::string(" ""blockid"": ");
                        answer += std::string("""") + at->getBlockId() + std::string(""",\n");
                        answer += std::string(" ""outputIndex"": ");
                        answer += std::to_string(at->getOutputIndex()) + std::string(",\n");
                        answer += std::string(" ""signature"": ");
                        answer += std::string("""") + at->getSignature() + std::string(""",\n");
                        answer += std::string(" ""txid"": ");
                        answer += std::string("""") + at->getTxid() + std::string(""",\n");

                        if (at + 1 == tempInTx.end()) {
                            answer += std::string(" }\n");
                        }
                        else {
                            answer += std::string(" },\n");
                        }

                    }
                    answer += std::string(" ],\n");

                    //Imprimo las VOUT
                    answer += std::string(" ""vout"": [ \n ");

                    for (std::vector<OutTx>::iterator at = tempOutTx.begin(); at != tempOutTx.end(); at++) {

                        answer += std::string(" {\n");
                        answer += std::string(" ""amount"": ");
                        answer += std::to_string(at->getAmount()) + std::string(",\n");
                        answer += std::string(" ""publicid"": ");
                        answer += std::string("""") + at->getPublicId() + std::string(""",\n");

                        if (at + 1 == tempOutTx.end()) {

                            answer += std::string(" }\n");
                        }
                        else {

                            answer += std::string(" },\n");
                        }
                    }
                    answer += std::string(" },\n");

                }

                i++;

                startCopying = false;
            }
        }
    }

    answer += std::string(" ""txPos"": ");
    answer += std::to_string(position) + std::string(",\n");
    answer += std::string(" ""merklePath"": [\n ");


    MerkleTree<hash32> mtree = MerkleTree<hash32>(merkleTxsIDs);
    std::vector <std::string> merklePaths;
    mtree.getMerklePath(txIDchosen, merklePaths);

    for (std::vector<std::string>::iterator it = merklePaths.begin(); it != merklePaths.end(); it++) {

        answer += std::string(" ""Id"": ");
        answer += std::string("""") + *it + std::string(""",\n");
    }

    answer += std::string(" ],\n}\n");

    commSend(host, std::string("eda_coin/send_merkle_block/"), answer);

    return false;
}

bool FullNode::getBlocks(std::string blockId, std::string blockCount, std::string host)
{
    char buf[50];

    sprintf_s(buf, "eda_coin/get_blocks?block_id=%s&count=%s", blockId.c_str(), blockCount.c_str());

    commSend(host, std::string(buf));

    return false;
}

std::string FullNode::blockPostReceived(bool error, int result)
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

    return answer;
}

std::string FullNode::transactionPostReceived(bool error, int result)
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

    return answer;
}

std::string FullNode::filterPostReceived(bool error, int result)
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

    return answer;
}

std::string FullNode::getBlockHeaderReceived(std::string blockID, int count)
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

    return answer;
}

std::string FullNode::getBlocksReceived(std::string blockID, int count)
{
    std::string answer = std::string("{ ""status"": true,\n ""result"": ");
    bool startCopying = false;
    int i = 0;
    std::vector<Tx> tempTx;
    std::vector<InTx> tempInTx;
    std::vector<OutTx> tempOutTx;

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
            tempTx = it->getTxVector();

            for (std::vector<Tx>::iterator ut = tempTx.begin(); ut != tempTx.end(); ut++) {
                
                answer += std::string(" {\n");
                answer += std::string(" ""nTxin"": ");
                answer += std::to_string(ut->getVin().size()) + std::string(",\n");
                answer += std::string(" ""nTxout"": ");
                answer += std::to_string(ut->getVout().size()) + std::string(",\n");
                answer += std::string("  ""txid"": ");
                answer += std::string("""") + ut->getId() + std::string(""",\n");


                //Imprimo las VIN
                answer += std::string(" ""vin"": [ \n ");
                tempInTx = ut->getVin();
                tempOutTx = ut->getVout();

                for (std::vector<InTx>::iterator at = tempInTx.begin(); at != tempInTx.end(); at++) {

                    answer += std::string(" {\n");
                    answer += std::string(" ""blockid"": ");
                    answer += std::string("""") + at->getBlockId() + std::string(""",\n");
                    answer += std::string(" ""outputIndex"": ");
                    answer += std::to_string(at->getOutputIndex()) + std::string(",\n");
                    answer += std::string(" ""signature"": ");
                    answer += std::string("""") + at->getSignature() + std::string(""",\n");
                    answer += std::string(" ""txid"": ");
                    answer += std::string("""") + at->getTxid() + std::string(""",\n");

                    if (at + 1 == tempInTx.end()) {
                        answer += std::string(" }\n");
                    }
                    else {
                        answer += std::string(" },\n");
                    }

                }
                answer += std::string(" ],\n") ;

                //Imprimo las VOUT
                answer += std::string(" ""vout"": [ \n ");

                for (std::vector<OutTx>::iterator at = tempOutTx.begin(); at != tempOutTx.end(); at++) {

                    answer += std::string(" {\n");
                    answer += std::string(" ""amount"": ");
                    answer += std::to_string(at->getAmount()) + std::string(",\n");
                    answer += std::string(" ""publicid"": ");
                    answer += std::string("""") + at->getPublicId() + std::string(""",\n");

                    if (at + 1 == tempOutTx.end()) {

                        answer += std::string(" }\n");
                    }
                    else {

                        answer += std::string(" },\n");
                    }
                }
                answer += std::string(" }\n],\n");
            }

            i++;
        }
        if ((it->getId()) == blockID) {

            startCopying = true;
        }
    }

    return answer;
}

std::string FullNode::receivedMsgCB(std::string client, std::string msg) 
{
    std::string answer; //Acá copiaré las respuestas que me den las funciones

    std::string path; //TODO: Acá guardo el path para averiguar de que se trata

    if ( path.find('?') == std::string::npos) {

        if (path == std::string("eda_coin/send_block/")) {

            if (msg != std::string("")) {

                //Aca podria por ejemplo hacer algo con el mensaje que nos enviaron
            }
            answer = blockPostReceived(false, 0);
        }
        else if (path == std::string("eda_coin/send_tx/")) {

            if (msg != std::string("")) {

                //Aca podria por ejemplo hacer algo con el mensaje que nos enviaron
            }
            answer = transactionPostReceived(false, 0);
        }
        else if (path == std::string("eda_coin/send_filter/")) {

            if (msg != std::string("")) {

                //Aca podria por ejemplo hacer algo con el mensaje que nos enviaron
            }
            answer = filterPostReceived(false, 0);
        }
    }
    else {

        std::string blockId = "01010101"; 
        int count = 0;

        //Tengo que conseguir los datos que estan en el comando

        if (path == std::string("get_block")) {
            
            answer = getBlocksReceived(blockId, count);
        }
        else if (path == std::string("get_block_header")) {

            answer = getBlockHeaderReceived(blockId, count);
        }
    }

    return answer;
}

std::vector<std::string> FullNode::getActionList()
{
    return actionsVector;
}
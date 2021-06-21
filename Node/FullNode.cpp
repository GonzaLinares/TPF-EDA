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

bool FullNode::blockPost(std::string host, std::string blockId)
{
    std::string answer = std::string("{");
    std::vector<Tx> tempTx;
    std::vector<InTx> tempInTx;
    std::vector<OutTx> tempOutTx;

    //Busco el bloque que me pidieron

    for (std::vector<Block>::iterator it = blockchain.begin(); it != blockchain.end(); it++) {

        if (blockId == it->getId()) {

            answer += std::string(" { \"blockid\": ");
            answer += std::string("\"") + it->getId() + std::string("\",\n");
            answer += std::string(" \"height\": ");
            answer += std::to_string(it->getHeight()) + std::string(",\n");
            answer += std::string(" \"merkleroot\": ");
            answer += std::string("\"") + it->getMerkleRoot() + std::string("\",\n");
            answer += std::string(" \"nTx\": ");
            answer += std::to_string(it->getnTx()) + std::string(",\n");
            answer += std::string(" \"nonce\": ");
            answer += std::to_string(it->getNonce()) + std::string(",\n");
            answer += std::string(" \"previousblockid\": ");
            answer += std::string("\"") + it->getPrevBlockId() + std::string("\",\n");
            answer += std::string(" \"tx\": [ \n ");
            tempTx = it->getTxVector();

            for (std::vector<Tx>::iterator ut = tempTx.begin(); ut != tempTx.end(); ut++) {

                answer += std::string(" {\n");
                answer += std::string(" \"nTxin\": ");
                answer += std::to_string(ut->getVin().size()) + std::string(",\n");
                answer += std::string(" \"nTxout\": ");
                answer += std::to_string(ut->getVout().size()) + std::string(",\n");
                answer += std::string("  \"txid\": ");
                answer += std::string("\"") + ut->getId() + std::string("\",\n");


                //Imprimo las VIN
                answer += std::string(" ""vin"": [ \n ");
                tempInTx = ut->getVin();
                tempOutTx = ut->getVout();

                for (std::vector<InTx>::iterator at = tempInTx.begin(); at != tempInTx.end(); at++) {

                    answer += std::string(" {\n");
                    answer += std::string(" \"blockid\": ");
                    answer += std::string("\"") + at->getBlockId() + std::string("\",\n");
                    answer += std::string(" \"outputIndex\": ");
                    answer += std::to_string(at->getOutputIndex()) + std::string(",\n");
                    answer += std::string(" \"signature\": ");
                    answer += std::string("\"") + at->getSignature() + std::string("\",\n");
                    answer += std::string(" ""txid"": ");
                    answer += std::string("\"") + at->getTxid() + std::string("\",\n");

                    if (at + 1 == tempInTx.end()) {
                        answer += std::string(" }\n");
                    }
                    else {
                        answer += std::string(" },\n");
                    }

                }
                answer += std::string(" ],\n");

                //Imprimo las VOUT
                answer += std::string(" \"vout\": [ \n ");

                for (std::vector<OutTx>::iterator at = tempOutTx.begin(); at != tempOutTx.end(); at++) {

                    answer += std::string(" {\n");
                    answer += std::string(" \"amount\": ");
                    answer += std::to_string(at->getAmount()) + std::string(",\n");
                    answer += std::string(" \"publicid\": ");
                    answer += std::string("\"") + at->getPublicId() + std::string("\",\n");

                    if (at + 1 == tempOutTx.end()) {

                        answer += std::string(" }\n");
                    }
                    else {

                        answer += std::string(" },\n");
                    }
                }

                if (ut + 1 == tempTx.end()) {
                    answer += std::string("]\n");
                }
                else{
                    answer += std::string("],\n");
                }
            }
        }
    }

    answer += std::string("}");

    commSend(host, std::string("eda_coin/send_block/"), answer);

    return false;
}

bool FullNode::transactionPost(std::string publicKey, int amount, std::string host)
{
    std::string answer;
    int VinCount = 0;

    int totalAmountInOutput = 0;

    for (std::vector<UTXO>::iterator at = MyUTXO.begin(); at != MyUTXO.end() && totalAmountInOutput < amount; at++) {

        totalAmountInOutput += at->getAmount();
        VinCount++;
    }
    if (totalAmountInOutput < amount) {

        //no te alcanza la plata
        return "";
    }
    else {

        totalAmountInOutput = 0; //La reinicio y empiezo de vuelta
    }

    //answer += std::string(" \"tx\": [ \n ");  //TODO esto no va aca.
    answer += std::string(" {\n");
    answer += std::string(" \"nTxin\": ");
    answer += std::to_string(VinCount);
    answer += std::string(",\n");
    answer += std::string(" \"nTxout\": |,\n"); //La cantidad de salidas va a ser siempre 2 o 1
    answer += std::string(" \"txid\": 00000000000000000000000000000000,\n");    //TODO: Despues lo reemplazo
    //VIN
    answer += std::string(" \"vin\": [ \n ");
    for (std::vector<UTXO>::iterator at = MyUTXO.begin(); at != MyUTXO.end() && totalAmountInOutput < amount; at++) {

        answer += std::string(" {\n");

        totalAmountInOutput += at->getAmount();

        answer += "\"blockid\": \"";
        answer += at->getBlockId();
        answer += "\",\n";

        answer += "\"outputIndex\": ";
        answer += at->getOutputIndex();
        answer += ",\n";

        answer += "\"signature\": \"";  
        //TODO: Agregar bien la firma
        answer += "\",\n";

        answer += "\"txid\": \"";
        answer += at->getTXId();
        answer += "\",\n";

        answer += std::string(" },\n");
    }

    answer += std::string(" ],\n");

    //VOUT
    answer += std::string(" \"vout\": [ \n ");
    answer += std::string(" {\n");
    answer += std::string(" \"amount\": ");
    answer += std::to_string(amount) + std::string(",\n");
    answer += std::string(" \"publicid\": ");
    answer += std::string("\"") + publicKey + std::string("\",\n");
    answer += std::string(" }\n");
    answer += std::string("],\n");

    if (totalAmountInOutput > amount) {
        answer += std::string(" {\n");
        answer += std::string(" \"amount\": ");
        answer += std::to_string(totalAmountInOutput-amount) + std::string(",\n");
        answer += std::string(" \"publicid\": ");
        answer += std::string("\"") + myID + std::string("\",\n");
        answer += std::string(" }\n");
        answer += std::string("]\n");
        answer.replace(answer.find_first_of('|'), 1, 1, '2');
    }
    else {

        answer.replace(answer.find_first_of('|'),1,1,'1');
    }

    

    answer += std::string("]\n");

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

            answer += std::string(" { \"blockid\": ");
            answer += std::string("\"") + it->getId() + std::string("\",\n");   
            answer += std::string(" \"tx\": { \n ");
            tempTx = it->getTxVector();

            for (std::vector<Tx>::iterator ut = tempTx.begin(); ut != tempTx.end(); ut++) {

                merkleTxsIDs.push_back(ut->getId());

                if (i == position) {

                    txIDchosen = ut->getId();

                    answer += std::string(" {\n");
                    answer += std::string(" \"nTxin\": ");
                    answer += std::to_string(ut->getVin().size()) + std::string(",\n");
                    answer += std::string(" \"nTxout\": ");
                    answer += std::to_string(ut->getVout().size()) + std::string(",\n");
                    answer += std::string("  \"txid\": ");
                    answer += std::string("\"") + ut->getId() + std::string("\",\n");

                    
                    //Imprimo las VIN
                    answer += std::string(" \"vin\": [ \n ");
                    tempInTx = ut->getVin();
                    tempOutTx = ut->getVout();

                    for (std::vector<InTx>::iterator at = tempInTx.begin(); at != tempInTx.end(); at++) {

                        answer += std::string(" {\n");
                        answer += std::string(" \"blockid\": ");
                        answer += std::string("\"") + at->getBlockId() + std::string("\",\n");
                        answer += std::string(" ""outputIndex"": ");
                        answer += std::to_string(at->getOutputIndex()) + std::string(",\n");
                        answer += std::string(" \"signature\": ");
                        answer += std::string("\"") + at->getSignature() + std::string("\",\n");
                        answer += std::string(" ""txid"": ");
                        answer += std::string("\"") + at->getTxid() + std::string("\",\n");

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
                        answer += std::string(" \"amount\": ");
                        answer += std::to_string(at->getAmount()) + std::string(",\n");
                        answer += std::string(" \"publicid\": ");
                        answer += std::string("\"") + at->getPublicId() + std::string("\",\n");

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

    answer += std::string(" \"txPos\": ");
    answer += std::to_string(position) + std::string(",\n");
    answer += std::string(" \"merklePath\": [\n ");

    if (blockchain.size() > 0) {
        MerkleTree<hash32> mtree = MerkleTree<hash32>(merkleTxsIDs);
        std::vector <std::string> merklePaths;
        mtree.getMerklePath(txIDchosen, merklePaths);

        for (std::vector<std::string>::iterator it = merklePaths.begin(); it != merklePaths.end(); it++) {

            answer += std::string(" \"Id\": ");
            answer += std::string("\"") + *it + std::string("\",\n");
        }

        answer += std::string(" ],\n}\n");

    }
    else {

        answer += std::string("There is no blockchain");
    }
    
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

            answer = std::string("{ \"status\": true,\n \"result\": 1 }");
        }
        else {

            answer = std::string("{ \"status\": true,\n \"result\": 2 }");
        }
    }
    else {

        answer = std::string("{ \"status\": true,\n \"result\": null }");
    }

    return answer;
}

std::string FullNode::transactionPostReceived(bool error, int result)
{
    std::string answer;

    if (error == true) {

        if (result == 1) {

            answer = std::string("{ \"status\": true,\n \"result\": 1 }");
        }
        else {

            answer = std::string("{ \"status\": true,\n \"result\": 2 }");
        }
    }
    else {

        answer = std::string("{ \"status\": true,\n \"result\": null }");
    }

    return answer;
}

std::string FullNode::filterPostReceived(bool error, int result)
{   
    std::string answer;

    if (error == true) {

        if (result == 1) {

            answer = std::string("{ \"status\": true,\n \"result\": 1 }");
        }
        else {

            answer = std::string("{ \"status\": true,\n \"result\": 2 }");
        }
    }
    else {

        answer = std::string("{ \"status\": true,\n \"result\": null }");
    }

    return answer;
}

std::string FullNode::getBlockHeaderReceived(std::string blockID, int count)
{

    std::string answer = std::string("");
    bool startCopying = false;
    int i = 0;

    answer = std::string("{ \"status\": true,\n \"result\": ");

    //Busco el bloque que me pidieron

    if (blockID == std::string("00000000")) {

        startCopying = true;
    }

    for (std::vector<Block>::iterator it = blockchain.begin(); it != blockchain.end() && i < count; it++) {

        if ((it->getId()) == blockID) {

            startCopying = true;
        }
        if (startCopying == true) {

            answer += std::string(" { \"blockid\": ");
            answer += std::string("\"") + it->getId() + std::string("\",\n");
            answer += std::string(" \"height\": ");
            answer += std::to_string(it->getHeight()) + std::string(",\n");
            answer += std::string(" \"merkleroot\": ");
            answer += std::string("\"") + it->getMerkleRoot() + std::string("\",\n");
            answer += std::string(" \"nTx\": ");
            answer += std::to_string(it->getnTx()) + std::string(",\n");
            answer += std::string(" \"nonce\": ");
            answer += std::to_string(it->getNonce()) + std::string(",\n");
            answer += std::string(" \"previousblockid\": ");
            answer += std::string("\"") + it->getPrevBlockId() + std::string("\",\n");
            if (i == count-1)
            {
                answer += std::string("}");
            }
            else
            {
                answer += std::string("},");
            }
            
            i++;
        }

    }

    if (startCopying == false)
    {
        answer.replace(answer.find("true"), 4, "false");
        answer += "NULL";
    }

    answer += std::string("}");

    return answer;
}

std::string FullNode::getBlocksReceived(std::string blockID, int count)
{
    std::string answer = std::string("{ \"status\": true,\n \"result\": ");
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

            answer += std::string(" { \"blockid\": ");
            answer += std::string("\"") + it->getId() + std::string("\",\n");
            answer += std::string(" \"height\": ");
            answer += std::to_string(it->getHeight()) + std::string(",\n");
            answer += std::string(" \"merkleroot\": ");
            answer += std::string("\"") + it->getMerkleRoot() + std::string("\",\n");
            answer += std::string(" \"nTx\": ");
            answer += std::to_string(it->getnTx()) + std::string(",\n");
            answer += std::string(" \"nonce\": ");
            answer += std::to_string(it->getNonce()) + std::string(",\n");
            answer += std::string(" \"previousblockid\": ");
            answer += std::string("\"") + it->getPrevBlockId() + std::string("\",\n");
            answer += std::string(" \"tx\": [ \n ");
            tempTx = it->getTxVector();

            for (std::vector<Tx>::iterator ut = tempTx.begin(); ut != tempTx.end(); ut++) {
                
                answer += std::string(" {\n");
                answer += std::string(" \"nTxin\": ");
                answer += std::to_string(ut->getVin().size()) + std::string(",\n");
                answer += std::string(" \"nTxout\": ");
                answer += std::to_string(ut->getVout().size()) + std::string(",\n");
                answer += std::string("  \"txid\": ");
                answer += std::string("\"") + ut->getId() + std::string("\",\n");


                //Imprimo las VIN
                answer += std::string(" \"vin\": [ \n ");
                tempInTx = ut->getVin();
                tempOutTx = ut->getVout();

                for (std::vector<InTx>::iterator at = tempInTx.begin(); at != tempInTx.end(); at++) {

                    answer += std::string(" {\n");
                    answer += std::string(" \"blockid\": ");
                    answer += std::string("\"") + at->getBlockId() + std::string("\",\n");
                    answer += std::string(" ""outputIndex"": ");
                    answer += std::to_string(at->getOutputIndex()) + std::string(",\n");
                    answer += std::string(" ""signature"": ");
                    answer += std::string("\"") + at->getSignature() + std::string("\",\n");
                    answer += std::string(" \"txid\": ");
                    answer += std::string("\"") + at->getTxid() + std::string("\",\n");

                    if (at + 1 == tempInTx.end()) {
                        answer += std::string(" }\n");
                    }
                    else {
                        answer += std::string(" },\n");
                    }

                }
                answer += std::string(" ],\n") ;

                //Imprimo las VOUT
                answer += std::string(" \"vout\": [ \n ");

                for (std::vector<OutTx>::iterator at = tempOutTx.begin(); at != tempOutTx.end(); at++) {

                    answer += std::string(" {\n");
                    answer += std::string(" \"amount\": ");
                    answer += std::to_string(at->getAmount()) + std::string(",\n");
                    answer += std::string(" \"publicid\": ");
                    answer += std::string("\"") + at->getPublicId() + std::string("\",\n");

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

void FullNode::validateTransactionPost(bool& error, int& result, std::string msg)
{   
    /*TODO: REALIZO TODO EL PARSEO Y GUARDO LAS COSAS ACA*/
    Tx tempTx("00000000");

    bool loEncontre = false;
    int totalAmountInUTXO = 0;
    int totalAmountInOutput = 0;

    //El HashID debe verificar  
    std::string hashTest = hexCodedAscii((tempTx.getVin()).size()) + hexCodedAscii((tempTx.getVout()).size());   //Armo un string para verificar la txID

    /*La UTXO referenciada en el Input Transaction de la Tx debe pertenecer al arreglo de UTXOs o 
    a las transacciones pendientes*/
    for (std::vector<InTx>::iterator it = (tempTx.getVin()).begin(); it != (tempTx.getVin()).end(); it++) {

        std::string blockid = it->getBlockId();
        std::string txID = it->getTxid();
        int outputIndex = it->getOutputIndex();

        hashTest += it->getBlockId() + hexCodedAscii(it->getOutputIndex()) + it->getSignature() + it->getTxid();

        //Le bailo rico a todas las inputs a ver si estan en el arreglo de UTXO, sino, a casona por cheater
        
        for (std::vector<UTXO>::iterator at = UTXOVector.begin(); at != UTXOVector.end() && loEncontre == false; at++) {

            if (at->getBlockId() == blockid) {

                if (at->getTXId() == txID) {

                    if (at->getOutputIndex() == outputIndex) {

                        loEncontre = true;
                        totalAmountInUTXO += at->getAmount();
                    }
                }
            }
        }
        if (loEncontre == false) {

            error = true;
            result = 2;
            return;
        }
    }

    /*La suma de los montos de EDACoin de los UTXOs referenciados en los
    Input Transactions tiene que coincidir con la suma de los montos de
    EDACoin referenciados en los Output Transactions.*/

    for (std::vector<OutTx>::iterator it = (tempTx.getVout()).begin(); it != (tempTx.getVout()).end(); it++) {

        totalAmountInOutput += it->getAmount();
        hashTest += hexCodedAscii(it->getAmount()) + it->getPublicId();
    }

    if (totalAmountInOutput != totalAmountInUTXO) {

        error = true;
        result = 2;
        return;
    }

    std::string hashTest1 = hash32(hashTest);

    if (hash32(hashTest1) != tempTx.getId()) {

        error = true;
        result = 2;
        return;
    }

    /*Los unlocking scripts referidos en cada Input Transaction deben
    efectivamente desbloquear los UTXO referidos en cada una de ellas*/


}

void FullNode::validateBlockPost(bool& error, int& result, std::string msg)
{

    std::string blockid = "!";              
    unsigned int height = 0;              
    std::string merkleroot = "!";             
    unsigned int nTx = 0;                    
    int nonce = 0;                            
    std::string previousblockid = "!";        

    Block blockSent(blockid, height, merkleroot, nTx, nonce, previousblockid);  //TODO: Hacer el parsing del bloque

    //Verificar que cumple con el challenge.
    std::string tempString;
    tempString = merkleroot + hexCodedAscii(nonce) + previousblockid;
    std::string tempString1 = hash32(tempString);
    tempString = hash32(tempString1);

    if (tempString != blockid) {

        error = true;
        result = 2;
        return;
    }

    tempString = blockid.substr(0, NUMBEROFZEROSCHALLENGE-1);
    for (int i = 0; i < 10; i++) {

        if (tempString[i] != '0') {
            error = true;
            result = 2;
            return;
        }
    }

    //El previous block hash coincide con el block hash del bloque anterior
    if (previousblockid == "0000000000000000000000000000000000000000000000000000000000000000" && currentBlock!= 0) {

        error = true;
        result = 2;
        return;
    }
    else if (previousblockid != (blockchain[currentBlock]).getId() ) {

        error = true;
        result = 2;
        return;
    }


    //TODO: todas las transacciones son válidas
}

void FullNode::validateFilterPost(bool& error, int& result, std::string msg)
{
    //Aca de momento no hariamos nada
}

std::string FullNode::receivedMsgCB(std::string client, std::string msg)
{
    std::string answer = std::string("damy value"); //Acá copiaré las respuestas que me den las funciones
    std::string host = client.substr(0, client.find_first_of('/', 0));
    lastReClient = host;

    client.erase(0, client.find_first_of('/', 0));

    bool error = false; //Parametros que serán enviados a las funciones de validación
    int result = 0;

        if (client == std::string("/eda_coin/send_block/")) {

            state = RECEIVING;
            if (msg != std::string("")) {

                validateBlockPost(error, result, msg);
                answer = blockPostReceived(error, result);
            }
        }
        else if (client == std::string("/eda_coin/send_tx/")) {

            state = RECEIVING;
            if (msg != std::string("")) {

                validateTransactionPost(error, result, msg);
                answer = transactionPostReceived(error, result);
            }
        }
        else if (client == std::string("/eda_coin/send_filter/")) {

            state = RECEIVING;
            if (msg != std::string("")) {

                validateFilterPost(error, result, msg);
                answer = filterPostReceived(error, result);
            }
        }
        else if (client == std::string("/eda_coin/get_blocks/")) {

            state = RECEIVING;
            std::string blockID = msg.substr(msg.find_first_of("blockid=", 0) + 9, 8);
            std::string temp = msg.substr(msg.find("count=", 0) + 6, msg.find_first_of('"', 5)); //TODO: Sirve mientras no haya ningun parametro despues de count
            answer = getBlocksReceived(blockID, stoi(temp));
        }
        else if (client == std::string("/eda_coin/get_block_header/")) {

            state = RECEIVING;
            std::string blockID = msg.substr(msg.find("blockid=", 0) + 10, 8);
            std::string temp = msg.substr(msg.find("count=", 0) + 6, msg.find_first_of('"', 5)); //TODO: Sirve mientras no haya ningun parametro despues de count
            answer = getBlockHeaderReceived(blockID, stoi(temp));
        }
        else
        {
#ifdef DEBUGCALLBACK
            std::cout << "Recieved at:" << getIP() + ":" + std::to_string(std::stoi(getPort())+1) << std::endl;
            std::cout << "*************************" << std::endl;
            std::cout << msg << std::endl;
            std::cout << "*************************" << std::endl << std::endl;
#endif // DEBUGCALLBACK
        }

    return answer;
}

std::vector<std::string> FullNode::getActionList()
{
    return actionsVector;
}
#include "FullNode.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include "..\MerkleTree.h"
#include "..\Hashing.h"
#include <cryptopp/eccrypto.h>
#include <cryptopp/hex.h>

using json = nlohmann::json;

std::vector<std::string> FullNode::actionsVector{ "BlockPost", "TransactionPost", "MerkleBlockPost", "GetBlocksPost" };
bool verifyMsg(std::string message, std::string signature, std::string pubKey);


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
    std::string messageVOUT;
    std::string message;
    std::string signa;
    std::string txIDPREHASH;
    int VinCount = 0;
    int totalAmountInOutput = 0;

    for (std::vector<UTXO>::iterator at = MyUTXO.begin(); at != MyUTXO.end() && totalAmountInOutput < amount; at++) {

        totalAmountInOutput += at->getAmount();
        VinCount++;
    }
    if (totalAmountInOutput < amount) {

        //no te alcanza la plata
        return false;
    }
    else if(totalAmountInOutput == amount){

        txIDPREHASH += std::to_string(VinCount);
        txIDPREHASH += "1";
        totalAmountInOutput = 0; //La reinicio y empiezo de vuelta
    }
    else {

        txIDPREHASH += std::to_string(VinCount);
        txIDPREHASH += "2";
        totalAmountInOutput = 0; //La reinicio y empiezo de vuelta
    }

    //Inicializo el messageVOUT para despues firmar

    messageVOUT += hexCodedAscii(amount) + publicKey;
    if (totalAmountInOutput > amount) {

        answer += std::to_string(totalAmountInOutput - amount) += myID;
    }

    //answer += std::string(" \"tx\": [ \n ");  //esto no va aca.
    answer += std::string(" {\n");
    answer += std::string(" \"nTxin\": ");
    answer += std::to_string(VinCount);
    answer += std::string(",\n");
    answer += std::string(" \"nTxout\": |,\n"); //La cantidad de salidas va a ser siempre 2 o 1 por como hacemos las cosas
    answer += std::string(" \"txid\": &000000000000000000000000000000%,\n");    //TODO: Despues lo reemplazo
    //VIN
    answer += std::string(" \"vin\": [ \n ");
    for (std::vector<UTXO>::iterator at = MyUTXO.begin(); at != MyUTXO.end() && totalAmountInOutput < amount; at++) {

        answer += std::string(" {\n");

        totalAmountInOutput += at->getAmount();

        answer += "\"blockid\": \"";
        answer += at->getBlockId();
        txIDPREHASH += at->getBlockId();
        answer += "\",\n";

        answer += "\"outputIndex\": ";
        answer += std::to_string(at->getOutputIndex());
        txIDPREHASH += std::to_string(at->getOutputIndex());
        answer += ",\n";

        answer += "\"signature\": \"";  
        answer += "@234567signatureplaceholder567812345678123456782525252525263515¿";
        answer += "\",\n";

        answer += "\"txid\": \"";
        answer += at->getTXId();
        answer += "\",\n";

        message.clear();
        message += at->getBlockId() + hexCodedAscii(at->getOutputIndex()) + at->getTXId();
        message += messageVOUT;
        signa = signMsg(message);
        answer.replace(answer.find_first_of('@'), answer.find_first_of('¿'), signa);

        txIDPREHASH += signa;
        txIDPREHASH += at->getTXId();

        answer += std::string(" },\n");
    }

    answer += std::string(" ],\n");

    //VOUT
    answer += std::string(" \"vout\": [ \n ");
    answer += std::string(" {\n");
    answer += std::string(" \"amount\": ");
    answer += std::to_string(amount) + std::string(",\n");
    txIDPREHASH += std::to_string(amount);
    answer += std::string(" \"publicid\": ");
    answer += std::string("\"") + publicKey + std::string("\",\n");
    txIDPREHASH += publicKey;
    answer += std::string(" }\n");
    answer += std::string("],\n");

    if (totalAmountInOutput > amount) {
        answer += std::string(" {\n");
        answer += std::string(" \"amount\": ");
        answer += std::to_string(totalAmountInOutput-amount) + std::string(",\n");
        txIDPREHASH += std::to_string(totalAmountInOutput - amount);
        answer += std::string(" \"publicid\": ");
        answer += std::string("\"") + myID + std::string("\",\n");
        txIDPREHASH += myID;
        answer += std::string(" }\n");
        answer += std::string("]\n");
        answer.replace(answer.find_first_of('|'), 1, 1, '2');
        txIDPREHASH += std::to_string(2);
    }
    else {

        answer.replace(answer.find_first_of('|'),1,1,'1');
        txIDPREHASH += std::to_string(1);
    }

    answer += std::string("}\n");

    //Ahora calculo la txid del comienzo
    txIDPREHASH = hash32(txIDPREHASH);
    txIDPREHASH = hash32(txIDPREHASH);

    answer.replace(answer.find_first_of('&'), answer.find_first_of('%'), txIDPREHASH);

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
    json jsonFile(msg);
    Tx tempTx(jsonFile["txid"]);

    for (int k = jsonFile["nTxin"] - 1; k >= 0; k--)
    {
        InTx auxInTx(jsonFile["vin"][k]["blockid"], jsonFile["vin"][k]["txid"], jsonFile["vin"][k]["signature"], jsonFile["vin"][k]["outputIndex"]);
        tempTx.push_vin(auxInTx);
    }

    for (int k = jsonFile["nTxout"] - 1; k >= 0; k--)
    {
        OutTx auxOutTx(jsonFile["vout"][k]["publicid"], jsonFile["vout"][k]["amount"]);
        tempTx.push_vout(auxOutTx);
    }

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

    std::string message;
    std::string messageVOUT;

    for (std::vector<OutTx>::iterator it = (tempTx.getVout()).begin(); it != (tempTx.getVout()).end(); it++) {

        messageVOUT += hexCodedAscii(it->getAmount()) + it->getPublicId();
    }

    for (std::vector<InTx>::iterator it = (tempTx.getVin()).begin(); it != (tempTx.getVin()).end(); it++) {

        std::string blockid = it->getBlockId();
        std::string txID = it->getTxid();
        int outputIndex = it->getOutputIndex();

        message += it->getBlockId() + hexCodedAscii(it->getOutputIndex()) + it->getTxid();
        message += messageVOUT;
        for (std::vector<UTXO>::iterator at = UTXOVector.begin(); at != UTXOVector.end() && loEncontre == false; at++) {

            if (at->getBlockId() == blockid) {

                if (at->getTXId() == txID) {

                    if (at->getOutputIndex() == outputIndex) {

                        if (! (verifyMsg( message, it->getSignature(), at->getPublicId() ))){

                            error = true;
                            result = 2;
                            return;
                        }
                    }
                }
            }
        }
        message.clear();
    }

    //TODO:Si todo esto salió bien tengo que añadir esta transacción a la siguiente minación. Creo que asi quedaria. Habria que ver de mandarla a minero
    verifiedTxs.push_back(tempTx);
}

void FullNode::validateBlockPost(bool& error, int& result, std::string msg)
{

    json jsonFile;

    std::string blockid = jsonFile["blockid"];
    unsigned int height = jsonFile["height"];
    std::string merkleroot = jsonFile["merkleroot"];
    unsigned int nTx = jsonFile["nTx"];
    int nonce = jsonFile["nonce"];
    std::string previousblockid = jsonFile["previousblockid"];

    Block blockSent(blockid, height, merkleroot, nTx, nonce, previousblockid);

    for (int j = jsonFile["nTx"] - 1; j >= 0; j--)
    {
        Tx auxTx(jsonFile["tx"][j]["txid"]);
        for (int k = jsonFile["tx"][j]["nTxin"] - 1; k >= 0; k--)
        {
            InTx auxInTx(jsonFile["tx"][j]["vin"][k]["blockid"], jsonFile["tx"][j]["vin"][k]["txid"], jsonFile["tx"][j]["vin"][k]["signature"], jsonFile["tx"][j]["vin"][k]["outputIndex"]);
            auxTx.push_vin(auxInTx);

        }

        for (int k = jsonFile["tx"][j]["nTxout"] - 1; k >= 0; k--)
        {
            OutTx auxOutTx(jsonFile["tx"][j]["vout"][k]["publicid"], jsonFile["tx"][j]["vout"][k]["amount"]);
            auxTx.push_vout(auxOutTx);
        }

        blockSent.push_transaction(auxTx);
    }

    //Verificar que cumple con el challenge.
    std::string tempString;
    tempString = previousblockid + merkleroot + hexCodedAscii(nonce);
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


    //Tiene que verificar toda esta cosita
    for (std::vector<Tx>::iterator it = (blockSent.getTxVector()).begin(); it != (blockSent.getTxVector()).end(); it++) {

        
        if (it->getId() != it->verifyTXID()) {

            error = true;
            result = 2;
            return;
        }
        

        std::string message;
        std::string messageVOUT;

        for (std::vector<OutTx>::iterator at = (it->getVout()).begin(); at != (it->getVout()).end(); at++) {

            messageVOUT += hexCodedAscii(at->getAmount()) + at->getPublicId();
        }

        for (std::vector<InTx>::iterator ut = (it->getVin()).begin(); ut != (it->getVin()).end(); ut++) {

            std::string blockid = ut->getBlockId();
            std::string txID = ut->getTxid();
            int outputIndex = ut->getOutputIndex();

            message += ut->getBlockId() + hexCodedAscii(ut->getOutputIndex()) + ut->getTxid();
            message += messageVOUT;
            for (std::vector<UTXO>::iterator at = UTXOVector.begin(); at != UTXOVector.end(); at++) {

                if (at->getBlockId() == blockid) {

                    if (at->getTXId() == txID) {

                        if (at->getOutputIndex() == outputIndex) {

                            if (!(verifyMsg(message, ut->getSignature(), at->getPublicId()))) {

                                error = true;
                                result = 2;
                                return;
                            }
                        }
                    }
                }
            }
            message.clear();
        }
    }

    blockchain.push_back(blockSent);
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

bool verifyMsg(std::string message, std::string signature, std::string pubKey) {
    /*
    * 
    * Codigo que no funciona
    * 
    CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey privateKeyTemp;
    CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey publicKeyTemp;

    std::string key;
    CryptoPP::publicKey.Save(CryptoPP::HexEncoder(new CryptoPP::StringSink(key)).Ref());  // Para almacenarlo en ASCII

    //Este es para el mensaje
    std::string destination;
    CryptoPP::StringSource ss(message, true, new CryptoPP::HexDecoder(new CryptoPP::StringSink(destination)));
    const CryptoPP::byte* result1 = (const CryptoPP::byte*)destination.data();

    //Este es para la firma
    std::string destination1;
    CryptoPP::StringSource ss1(it->getSignature(), true, new CryptoPP::HexDecoder(new CryptoPP::StringSink(destination1)));
    const CryptoPP::byte* result2 = (const CryptoPP::byte*)destination1.data();

    if (verifierTemp.VerifyMessage(result1, message.length(), result2, (it->getSignature()).length()) == false) {

        error = true;
        result = 2;
        return;
    }
    */

    CryptoPP::StringSource pubKeyHex(pubKey, true, new CryptoPP::HexDecoder);

    CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey pub;
    pub.AccessGroupParameters().SetPointCompression(true);

    pub.Load(pubKeyHex);

    CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Verifier verifier(pub);

    std::string signHex;
    CryptoPP::StringSource ss(signature, true, new CryptoPP::HexDecoder(new CryptoPP::StringSink(signHex)));

    return verifier.VerifyMessage((CryptoPP::byte*)message.data(), message.size(), (CryptoPP::byte*)signHex.data(), signHex.size());
}

std::string FullNode::signMsg(std::string msg) {

    CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Signer signer(privateKey1);

    size_t siglen = signer.MaxSignatureLength();
    std::string signature(siglen, 0x00);

    signer.SignMessage(randomnumbers, (CryptoPP::byte*)msg.data(), msg.size(), (CryptoPP::byte*)signature.data());
    signature.resize(siglen);

    std::string signStr;
    CryptoPP::StringSource ss(signature, true, new CryptoPP::HexEncoder(new CryptoPP::StringSink(signStr)));  // de Hex a ASCII

    return signStr;
}
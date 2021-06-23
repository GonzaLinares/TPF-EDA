#include "Tx.h"
#include "../Hashing.h"
#include "UTXO.h"


Tx::Tx(std::string txid_) :
	txid(txid_)
{
}

Tx::Tx()
{
    txid = "EMPTY";
}

void Tx::push_vin(InTx& newInTx)
{
	vin.push_back(newInTx);
}

void Tx::push_vout(OutTx& newOutTx)
{
	vout.push_back(newOutTx);
}

std::string Tx::getId(void)
{
	return txid;
}

std::vector<InTx> Tx::getVin(void)
{
	return vin;
}

std::vector<OutTx> Tx::getVout(void)
{
	return vout;
}

std::string Tx::calculateTXID()
{
    std::string hashTest = hexCodedAscii((getVin()).size()) + hexCodedAscii((getVout()).size());   //Armo un string para verificar la txID

    /*La UTXO referenciada en el Input Transaction de la Tx debe pertenecer al arreglo de UTXOs o
    a las transacciones pendientes*/
    for (auto it : vin) { 

        hashTest += it.getBlockId() + hexCodedAscii(it.getOutputIndex()) + it.getSignature() + it.getTxid();
    }
    
    for (auto it : vout) {

        hashTest += hexCodedAscii(it.getAmount()) + it.getPublicId();
    }


    hashTest = hash32(hashTest);
    txid = hash32(hashTest);

    return txid;
}

std::string Tx::verifyTXID()
{
    std::string hashTest = hexCodedAscii((getVin()).size()) + hexCodedAscii((getVout()).size());   //Armo un string para verificar la txID

    /*La UTXO referenciada en el Input Transaction de la Tx debe pertenecer al arreglo de UTXOs o
    a las transacciones pendientes*/
    for (auto it : vin) {

        hashTest += it.getBlockId() + hexCodedAscii(it.getOutputIndex()) + it.getSignature() + it.getTxid();
    }

    for (auto it : vout) {

        hashTest += hexCodedAscii(it.getAmount()) + it.getPublicId();
    }


    hashTest = hash32(hashTest);
    std::string txid111 = hash32(hashTest);

    return txid111;
}

std::string Tx::dump2JSON()		
{
    std::string answer; //Enrealidadnosesiestabienasi

    //answer += std::string(" \"tx\": [ \n ");  //esto no va aca.
    answer += std::string(" {\n");
    answer += std::string(" \"nTxin\": ");
    answer += std::to_string(this->getVin().size());
    answer += std::string(",\n");
    answer += std::string(" \"nTxout\":");
    answer += std::to_string(this->getVin().size()) + " ,\n";
    answer += std::string(" \"txid\": &000000000000000000000000000000%,\n");    //TODO: Despues lo reemplazo
    //VIN
    answer += std::string(" \"vin\": [ \n ");
    for (auto at : vin) {

        answer += std::string(" {\n");

        answer += "\"blockid\": \"";
        answer += at.getBlockId();
        answer += "\",\n";

        answer += "\"outputIndex\": ";
        answer += std::to_string(at.getOutputIndex());
        answer += ",\n";

        answer += "\"signature\": \"";
        answer += at.getSignature();
        answer += "\",\n";

        answer += "\"txid\": \"";
        answer += at.getTxid();
        answer += "\",\n";

        if (&at == &this->getVin().back()) {

            answer += std::string(" }\n");
        }
        else {
            answer += std::string(" },\n");
        }
    }

    answer += std::string(" ],\n");

    //VOUT
    answer += std::string(" \"vout\": [ \n ");
    for (auto at : vout) {

        answer += std::string(" {\n");

        answer += "\"amount\": \"";
        answer += hexCodedAscii((at.getAmount()));
        answer += "\",\n";

        answer += "\"publicid\": ";
        answer += at.getPublicId();
        answer += " \n";

        if (&at == &this->getVout().back()) {

            answer += std::string(" }\n");
        }
        else {
            answer += std::string(" },\n");
        }
    }

    answer += std::string("}\n");

    return answer;
}

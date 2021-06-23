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

    return hash32(hashTest);
}

std::string Tx::dump2JSON()		
{
    return "se implementará en la version 2.0";
}

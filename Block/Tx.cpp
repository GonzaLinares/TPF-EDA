#include "Tx.h"
#include "../Hashing.h"

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

void Tx::calculateTXID()
{
    std::string hashTest = hexCodedAscii((getVin()).size()) + hexCodedAscii((getVout()).size());   //Armo un string para verificar la txID

    /*La UTXO referenciada en el Input Transaction de la Tx debe pertenecer al arreglo de UTXOs o
    a las transacciones pendientes*/
    for (std::vector<InTx>::iterator it = (getVin()).begin(); it != (getVin()).end(); it++) {

        std::string blockid = it->getBlockId();
        std::string txID = it->getTxid();
        int outputIndex = it->getOutputIndex();

        hashTest += it->getBlockId() + hexCodedAscii(it->getOutputIndex()) + it->getSignature() + it->getTxid();
    }

    for (std::vector<OutTx>::iterator it = (getVout()).begin();  it != (getVout()).end(); it++) {

        hashTest += hexCodedAscii(it->getAmount()) + it->getPublicId();
    }


    hashTest = hash32(hashTest);
    txid = hash32(hashTest);
}

std::string Tx::dump2JSON()		//TODO: Implementar
{


	return "Implementar";	
}

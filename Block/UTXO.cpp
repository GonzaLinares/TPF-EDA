#include "UTXO.h"

UTXO::UTXO() {
	amount = -1;
	blockId = "Empty";
	txId = "Empty";
	publicId = "Empty";
	outputIndex = -1;
}

UTXO::UTXO(float amount, std::string blockId, std::string txId, std::string publicId, int outputIndex)
{
	this->amount = amount;
	this->blockId = blockId;
	this->txId = txId;
	this->publicId = publicId;
	this->outputIndex = outputIndex;
}

std::string UTXO::getBlockId(void)
{
	return blockId;
}

std::string UTXO::getTXId(void)
{
	return txId;
}

std::string UTXO::getPublicId(void)
{
	return publicId;
}

int UTXO::getOutputIndex(void)
{
	return outputIndex;
}

float UTXO::getAmount()
{
	return amount;
}

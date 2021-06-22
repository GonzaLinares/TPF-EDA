#include "UTXO.h"

UTXO::UTXO() {
	amount = -1;
	blockId = "Empty";
	txId = "Empty";
	publicId = "Empty";
	outputIndex = -1;
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

int UTXO::getAmount()
{
	return amount;
}

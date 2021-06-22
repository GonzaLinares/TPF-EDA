#include "UTXO.h"

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

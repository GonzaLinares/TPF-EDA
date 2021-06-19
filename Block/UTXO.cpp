#include "UTXO.h"

std::string UTXO::getBlockId(void)
{
	return blockId;
}

std::string UTXO::getTXId(void)
{
	return txId;
}

int UTXO::getOutputIndex(void)
{
	return outputIndex;
}

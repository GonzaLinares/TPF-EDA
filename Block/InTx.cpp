#include "InTx.h"

InTx::InTx(std::string blockid_, std::string txid_, std::string signature_, int outputindex_) :
	blockId(blockid_),
	txid(txid_),
	signature(signature_),
	outputIndex(outputindex_)
{

}

std::string InTx::getBlockId(void)
{
	return blockId;
}

std::string InTx::getTxid(void)
{
	return txid;
}

std::string InTx::getSignature(void)
{
	return signature;
}

int InTx::getOutputIndex(void)
{
	return outputIndex;
}

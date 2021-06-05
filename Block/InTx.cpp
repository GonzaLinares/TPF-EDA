#include "InTx.h"

InTx::InTx(std::string blockid_, std::string txid_, std::string signature_, int outputindex_) :
	blockId(blockid_),
	txid(txid_),
	signature(signature_),
	outputIndex(outputindex_)
{

}

#include "Block.h"

Block::Block(std::string blockid_, unsigned int height_, std::string merkleRoot_, unsigned int nTx_, int nonce_, std::string prevblockid_) :
	blockId(blockid_),
	height(height_),
	nonce(nonce_),
	previousBlockId(prevblockid_),
	merkleRoot(merkleRoot_)
{
}

bool Block::push_transaction(Tx newTx)
{
	txs.push_back(newTx);
	return true;
}

void Block::getMerklePath(void)
{


}

std::string Block::getBlockId(void)
{
	return blockId;
}

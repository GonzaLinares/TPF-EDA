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

std::string Block::getMerkleRoot(void)
{
	return merkleRoot;
}

std::string Block::getId(void)
{
	return blockId;;
}

unsigned int Block::getHeight(void)
{
	return height;
}

unsigned int Block::getNonce(void)
{
	return nonce;
}

std::string Block::getPrevBlockId(void)
{
	return previousBlockId;
}

unsigned int Block::getnTx(void)
{
	return txs.size();
}

bool Block::getTxsID(std::vector<std::string>& buffer, int numOfTxs, int offset)	//TODO: Que verga pasaba si numOfBlocks es 0 
{
    if (txs.size() == 0)
    {
        return false;
    }

    if (txs.size() - offset > numOfTxs)
    {
        numOfTxs = txs.size() - offset;
    }

    std::vector<Tx>::reverse_iterator it = txs.rbegin();

    if (numOfTxs == 0 && offset == 0)
    {
        for (int i = 1; i <= txs.size(); it++, i++) {

            buffer.push_back(it->getId());
        }
    }
    else
    {
        it += offset;

        for (int i = 1; i <= numOfTxs; it++, i++) {

            buffer.push_back(it->getId());
        }
    }


    return true;
}

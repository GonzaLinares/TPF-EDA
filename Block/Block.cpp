#include "Block.h"
#include <string>

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

bool Block::getTxsID(std::vector<std::string>& buffer, int numOfTxs, int offset)
{

    bool result = true;
    if (txs.size() == 0)
    {
        return false;
    }

    if ((int)txs.size() - offset < numOfTxs)
    {
        numOfTxs = txs.size() - offset;
        result = false;
    }

    std::vector<Tx>::reverse_iterator it = txs.rbegin();

    if (numOfTxs == 0 && offset == 0)
    {
        for (int i = 1; i <= (int)txs.size(); it++, i++) {

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


    return result;
}

std::vector<Tx> Block::getTxVector(void)
{

    return txs;
}

std::string Block::dump2JSON()
{
    std::string answer;
    std::vector<Tx> tempTx;
    std::vector<InTx> tempInTx;
    std::vector<OutTx> tempOutTx;

    answer += std::string(" { \"blockid\": ");
    answer += std::string("\"") + this->getId() + std::string("\",\n");
    answer += std::string(" \"height\": ");
    answer += std::to_string(this->getHeight()) + std::string(",\n");
    answer += std::string(" \"merkleroot\": ");
    answer += std::string("\"") + this->getMerkleRoot() + std::string("\",\n");
    answer += std::string(" \"nTx\": ");
    answer += std::to_string(this->getnTx()) + std::string(",\n");
    answer += std::string(" \"nonce\": ");
    answer += std::to_string(this->getNonce()) + std::string(",\n");
    answer += std::string(" \"previousblockid\": ");
    answer += std::string("\"") + this->getPrevBlockId() + std::string("\",\n");
    answer += std::string(" \"tx\": [ \n ");
    tempTx = this->getTxVector();

    for (std::vector<Tx>::iterator ut = tempTx.begin(); ut != tempTx.end(); ut++) {

        answer += std::string(" {\n");
        answer += std::string(" \"nTxin\": ");
        answer += std::to_string(ut->getVin().size()) + std::string(",\n");
        answer += std::string(" \"nTxout\": ");
        answer += std::to_string(ut->getVout().size()) + std::string(",\n");
        answer += std::string("  \"txid\": ");
        answer += std::string("\"") + ut->getId() + std::string("\",\n");


        //Imprimo las VIN
        answer += std::string(" ""vin"": [ \n ");
        tempInTx = ut->getVin();
        tempOutTx = ut->getVout();

        for (std::vector<InTx>::iterator at = tempInTx.begin(); at != tempInTx.end(); at++) {

            answer += std::string(" {\n");
            answer += std::string(" \"blockid\": ");
            answer += std::string("\"") + at->getBlockId() + std::string("\",\n");
            answer += std::string(" \"outputIndex\": ");
            answer += std::to_string(at->getOutputIndex()) + std::string(",\n");
            answer += std::string(" \"signature\": ");
            answer += std::string("\"") + at->getSignature() + std::string("\",\n");
            answer += std::string(" ""txid"": ");
            answer += std::string("\"") + at->getTxid() + std::string("\",\n");

            if (at + 1 == tempInTx.end()) {
                answer += std::string(" }\n");
            }
            else {
                answer += std::string(" },\n");
            }

        }
        answer += std::string(" ],\n");

        //Imprimo las VOUT
        answer += std::string(" \"vout\": [ \n ");

        for (std::vector<OutTx>::iterator at = tempOutTx.begin(); at != tempOutTx.end(); at++) {

            answer += std::string(" {\n");
            answer += std::string(" \"amount\": ");
            answer += std::to_string(at->getAmount()) + std::string(",\n");
            answer += std::string(" \"publicid\": ");
            answer += std::string("\"") + at->getPublicId() + std::string("\",\n");

            if (at + 1 == tempOutTx.end()) {

                answer += std::string(" }\n");
            }
            else {

                answer += std::string(" },\n");
            }
        }

        if (ut + 1 == tempTx.end()) {
            answer += std::string("]\n");
        }
        else {
            answer += std::string("],\n");
        }
    }
    answer += std::string("}");

    return answer;
}

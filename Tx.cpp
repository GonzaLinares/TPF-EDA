#include "Tx.h"

Tx::Tx(std::string txid_, unsigned int nTxin_, unsigned int nTxout_) :
	txid(txid_),
	nTxin(nTxin_),
	nTxout(nTxout_)
{

}

bool Tx::push_vin(InTx& newInTx)
{
	vin.push_back(newInTx);

	return true;
}

bool Tx::push_vout(OutTx& newOutTx)
{
	vout.push_back(newOutTx);

	return true;
}

std::string Tx::getId(void)
{
	return txid;
}

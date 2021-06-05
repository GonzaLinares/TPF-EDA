#include "Tx.h"

Tx::Tx(std::string txid_) :
	txid(txid_)
{

}

void Tx::push_vin(InTx& newInTx)
{
	vin.push_back(newInTx);
}

void Tx::push_vout(OutTx& newOutTx)
{
	vout.push_back(newOutTx);
}

std::string Tx::getId(void)
{
	return txid;
}

std::vector<InTx> Tx::getVin(void)
{
	return vin;
}

std::vector<OutTx> Tx::getVout(void)
{
	return vout;
}

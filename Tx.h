#pragma once

#include <iostream>
#include <vector>
#include "InTx.h"
#include "OutTx.h"

class Tx
{
public:

	Tx(std::string txid_, unsigned int nTxin_, unsigned int nTxout_);

	bool push_vin(InTx&);
	bool push_vout(OutTx&);
	std::string getId(void);

private:

	std::string txid;
	unsigned int nTxin;
	unsigned int nTxout;

	std::vector <InTx> vin;
	std::vector <OutTx> vout;
};


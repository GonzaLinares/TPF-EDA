#pragma once

#include <iostream>
#include <vector>
#include "InTx.h"
#include "OutTx.h"

class Tx
{

private:

	std::string txid;
	unsigned int nTxin;
	unsigned int nTxout;

	std::vector <InTx> vin;
	std::vector <OutTx> vout;
};


#pragma once

#include <iostream>

class UTXO
{
public:

	std::string getBlockId(void);
	std::string getTXId(void);
	int getOutputIndex(void);
	int getAmount();

private:

	int amount;
	std::string blockId;
	std::string txId;
	int outputIndex;
};


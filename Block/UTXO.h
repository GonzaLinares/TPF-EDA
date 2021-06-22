#pragma once

#include <iostream>

class UTXO
{
public:

	UTXO();
	UTXO(int amount, std::string blockId, std::string txId, std::string publicId, int outputIndex);
	std::string getBlockId(void);
	std::string getTXId(void);
	std::string getPublicId(void);
	int getOutputIndex(void);
	int getAmount();

private:
	
	int amount;
	std::string blockId;
	std::string txId;
	std::string publicId;
	int outputIndex;
};


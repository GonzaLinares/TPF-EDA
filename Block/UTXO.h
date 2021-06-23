#pragma once

#include <iostream>

class UTXO
{
public:

	UTXO();
	UTXO(float amount, std::string blockId, std::string txId, std::string publicId, int outputIndex);
	std::string getBlockId(void);
	std::string getTXId(void);
	std::string getPublicId(void);
	int getOutputIndex(void);
	float getAmount();

private:
	
	float amount;
	std::string blockId;
	std::string txId;
	std::string publicId;
	int outputIndex;
};


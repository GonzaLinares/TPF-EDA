#pragma once

#include <iostream>

class InTx
{

public:
	InTx(std::string blockid_, std::string txid_, std::string signature_, int outputindex_);

private:
	std::string blockId;
	std::string txid;
	std::string signature;

	int outputIndex;



};


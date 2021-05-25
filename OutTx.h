#pragma once

#include <iostream>

class OutTx
{
public:
	OutTx(std::string publicId_, int amount_);

private:
	std::string publicId;
	int amount;
};


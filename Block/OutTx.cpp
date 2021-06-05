#include "OutTx.h"

OutTx::OutTx(std::string publicId_, int amount_) :
	publicId(publicId_),
	amount(amount_)
{
}

std::string OutTx::getPublicId(void)
{
	return publicId;
}

int OutTx::getAmount(void)
{
	return amount;
}

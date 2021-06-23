#include "OutTx.h"

OutTx::OutTx(std::string publicId_, float amount_) :
	publicId(publicId_),
	amount(amount_)
{
}

std::string OutTx::getPublicId(void)
{
	return publicId;
}

float OutTx::getAmount(void)
{
	return amount;
}

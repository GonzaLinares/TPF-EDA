#pragma once

/***************************************************************************//**
@file     +InTx.h+
@brief    +Header+
@author   +Grupo 9+
******************************************************************************/

/******************************************************************************
* INCLUDE HEADER FILES
******************************************************************************/
#include <iostream>

/******************************************************************************
* CLASS InTx NODE
******************************************************************************/
class InTx
{

public:

	/*=====================================================
	* Name: constructor
	* Entra: Recibe los datos con los que va a llenar el bloque
	* Resulta: -
	*=====================================================*/
	InTx(std::string blockid_, std::string txid_, std::string signature_, int outputindex_);

	std::string getBlockId(void);
	std::string getTxid(void);
	std::string getSignature(void);
	int getOutputIndex(void);

private:

	std::string blockId;
	std::string txid;
	std::string signature;
	int outputIndex;
};


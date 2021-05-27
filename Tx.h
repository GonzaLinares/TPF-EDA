#pragma once

/***************************************************************************//**
@file     +Tx.h+
@brief    +Header+
@author   +Grupo 9+
******************************************************************************/

/******************************************************************************
* INCLUDE HEADER FILES
******************************************************************************/
#include <iostream>
#include <vector>
#include "InTx.h"
#include "OutTx.h"


/******************************************************************************
* CLASS TX NODE
******************************************************************************/
class Tx
{
public:

	/*=====================================================
	* Name: constructor
	* Entra: -
	* Resulta: 
	*=====================================================*/
	Tx(std::string txid_, unsigned int nTxin_, unsigned int nTxout_);


	/*=====================================================
	* Name: push_vin
	* Entra: -
	* Resulta: 
	*=====================================================*/
	bool push_vin(InTx&);

	/*=====================================================
	* Name: push_vout
	* Entra: -
	* Resulta: 
	*=====================================================*/
	bool push_vout(OutTx&);

	/*=====================================================
	* Name: getID
	* Entra: -
	* Resulta: Devuelve el ID de la transaccion
	*=====================================================*/
	std::string getId(void);

private:

	std::string txid;
	unsigned int nTxin;
	unsigned int nTxout;

	std::vector <InTx> vin;
	std::vector <OutTx> vout;
};


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
	* Entra: Id de la transaccion
	* Resulta: contruye un objeto
	*=====================================================*/
	Tx(std::string txid_);


	/*=====================================================
	* Name: push_vin
	* Entra: referencia al objeto a aniadir
	* Resulta: Aniade una entrada a la transaccion
	*=====================================================*/
	void push_vin(InTx&);

	/*=====================================================
	* Name: push_vout
	* Entra: referencia al objeto a aniadir
	* Resulta: aniade una salida a la transaccion
	*=====================================================*/
	void push_vout(OutTx&);

	/*=====================================================
	* Name: getID
	* Entra: -
	* Resulta: Devuelve el ID de la transaccion
	*=====================================================*/
	std::string getId(void);

	/*=====================================================
	* Name: getVin
	* Entra: -
	* Resulta: 
	*=====================================================*/
	std::vector <InTx> getVin(void);

	/*=====================================================
	* Name: getVout
	* Entra: -
	* Resulta:
	*=====================================================*/
	std::vector <OutTx> getVout(void);

	std::string dump2JSON();

private:

	std::string txid;		// Id de la transacicon
	std::vector <InTx> vin;	//vector de entradas
	std::vector <OutTx> vout;	//vectro de salidas
};


#pragma once

/***************************************************************************//**
@file     +Block.h+
@brief    +Clase bloque+
@author   +Grupo 9+
******************************************************************************/

/******************************************************************************
* INCLUDE HEADER FILES
******************************************************************************/
#include "Tx.h"
#include <iostream>
#include <vector>


/******************************************************************************
* CLASS BLOCK
******************************************************************************/
class Block
{

public:

	/*=====================================================
	* Name: Constructor
	* Entra: Id, altura, mekleroot, numero de transacciones, nonce. y Id del bloque anterior
	* Resulta: -
	*=====================================================*/
	Block(std::string blockid_, unsigned int height_, std::string merkleRoot_, unsigned int nTx_, int nonce_, std::string prevblockid_);

	/*=====================================================
	* Name: push_transaction
	* Entra: Transaccion
	* Resulta: Aniade la transaccion al bloque
	*=====================================================*/
	bool push_transaction(Tx);

	/*=====================================================
	* Name: getMerkleRoot
	* Entra: -
	* Resulta: obtiene la merkle root guardada en el bloque. No la calcula
	*=====================================================*/
	std::string getMerkleRoot(void);

	/*=====================================================
	* Name: getId
	* Entra: -
	* Resulta: Devuelve la id del bloque
	*=====================================================*/
	std::string getId(void);

	/*=====================================================
	* Name: getHeight
	* Entra: -
	* Resulta: Devuelve la altura del bloque
	*=====================================================*/
	unsigned int getHeight(void);

	/*=====================================================
	* Name: getNonce
	* Entra: -
	* Resulta: Devuelve el nonce del bloque
	*=====================================================*/
	unsigned int getNonce(void);

	/*=====================================================
	* Name: getPrevBlockId
	* Entra: -
	* Resulta: Devuelve la id del bloque previo
	*=====================================================*/
	std::string getPrevBlockId(void);

	/*=====================================================
	* Name: getnTx
	* Entra: -
	* Resulta: Devuelve la cantidad de transacciones del bloque
	*=====================================================*/
	unsigned int getnTx(void);

	/*=====================================================
	* Name: getTxsID
	* Entra: vector de strings donde se guardaran las IDs de las transacciones del bloque, numero de transacciones que
	* se desean y offset referenciado a la primer transaccion. Si no se recibe numero de transacciones se cargaran todas las disponibles
	* Resulta: true o false dependiendo si se pudieron leer todas las transacciones o solo algunas.
	* Si la cantidad solicitada es mayor a la disponible se devuelven los que esten y false.
	*=====================================================*/
	bool getTxsID(std::vector<std::string>& buffer, int numOfTxs = 0, int offset = 0);

	/*=====================================================
	* Name: getTxVector
	* Entra: 
	* Resulta: 
	*=====================================================*/
	std::vector<Tx> getTxVector(void);

	std::string dump2JSON();

private:

	std::string blockId;		//Id del bloque
	unsigned int height;		//Altura del bloque
	unsigned int nonce;			//Nonce del bloque
	std::string previousBlockId;	//Id del bloque previo
	std::string merkleRoot;		//Merkleroot del bloque
	std::vector<Tx> txs;		//Vector con las transacciones.

};


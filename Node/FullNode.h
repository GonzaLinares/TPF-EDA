#pragma once
/***************************************************************************//**
@file     +FullNode.h+
@brief    +Header hijo+
@author   +Grupo 9+
******************************************************************************/


/******************************************************************************
* INCLUDE HEADER FILES
******************************************************************************/
#include "../Block/Block.h"
#include "BaseNode.h"
#include <vector>
#include "../Block/UTXO.h"


/******************************************************************************
* CLASS FULLNODE
******************************************************************************/
class FullNode : public BaseNode
{

public:


	FullNode(boost::asio::io_context& ioContext, std::string port, std::string path2blockchain);
	FullNode(boost::asio::io_context& ioContext, std::string port);
	// Inherited via BaseNode
	// override specifier ensures that the function is virtual and is overriding a virtual function from a base class

	virtual bool createBlockchainFromFile(std::string&) override;
	virtual std::string getNodeType() override;
		
	//POST HTTP	*********************************************************
	bool blockPost(std::string host, std::string blockId);
	bool transactionPost(std::string publicKey, int amount, std::string host);
	bool merkleBlockPost(std::string blockId, int position, std::string host);

	//*********************************************************//

	//GET HTTP	*********************************************************
	bool getBlocks(std::string blockId, std::string blockCount, std::string host);

	//*********************************************************//

	//FUNCIONES RESPUESTA DE POSTS Y GETS *********************************************************

	/*=====================================================
	* Name: blockPostReceived
	* Entra: error-> Tiene que ser = true si hubo error
	* result-> En el caso de que hubo error, mandas 1 si hubo error de formato y 2 si hubo error de contenido
	* Resulta: -
	*=====================================================*/
	std::string blockPostReceived(bool error, int result);

	std::string transactionPostReceived(bool error, int result);

	std::string filterPostReceived(bool error, int result);

	/*=====================================================
	* Name: getBlockHeaderReceived
	* Entra: blockID-> 
	* count-> 
	* host->
	* Resulta: -
	*=====================================================*/
	std::string getBlockHeaderReceived(std::string blockID, int count);

	std::string getBlocksReceived(std::string blockID, int count);

	//*********************************************************//

	std::string signMessage(std::string msg);

protected:
	
	void validateTransactionPost(bool& error, int& result, std::string msg);
	void validateBlockPost(bool& error, int& result, std::string msg);
	void validateFilterPost(bool& error, int& result, std::string msg);

	std::string receivedMsgCB(std::string client, std::string msg);

	static std::vector<std::string> actionsVector;
	virtual std::vector<std::string> getActionList() override;	//Vector con los nombres de las acciones posibles
	std::vector<UTXO> UTXOVector;	//Las UTXO de la blockchain

	std::string myID;

	std::vector<Tx> validatedTransactions;

	CryptoPP::RandomNumberGenerator randomnumbers;
};

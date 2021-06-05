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


/******************************************************************************
* CLASS FULLNODE
******************************************************************************/
class FullNode : public BaseNode
{

public:


	FullNode(boost::asio::io_context& ioContext, std::string path2blockchain, std::string port);
	FullNode(boost::asio::io_context& ioContext, std::string port);
	// Inherited via BaseNode
	// override specifier ensures that the function is virtual and is overriding a virtual function from a base class

	virtual bool createBlockchainFromFile(std::string&) override;

		
	//POST HTTP	*********************************************************
	bool blockPost(std::string host);
	bool transactionPost(std::string publicKey, int amount, std::string host);
	bool merkleBlockPost(std::string blockId, std::string host, int position);

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
	bool blockPostReceived(bool error, int result, std::string host);

	bool transactionPostReceived(bool error, int result, std::string host);

	bool filterPostReceived(bool error, int result, std::string host);

	/*=====================================================
	* Name: getBlockHeaderReceived
	* Entra: blockID-> 
	* count-> 
	* host->
	* Resulta: -
	*=====================================================*/
	bool getBlockHeaderReceived(std::string blockID, int count, std::string host);

	bool getBlocksReceived(std::string blockID, int count, std::string host);

	//*********************************************************//

private:
	
	std::string receivedMsgCB(std::string client, std::string msg);

	static std::vector<std::string> actionsVector;
	virtual std::vector<std::string> getActionList() override;
	//Vector con los nombres de las acciones posibles
};

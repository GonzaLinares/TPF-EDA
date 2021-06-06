#pragma once
/***************************************************************************//**
@file     +BaseNode.h+
@brief    +Header padre+
@author   +Grupo 9+
******************************************************************************/

/******************************************************************************
* INCLUDE HEADER FILES
******************************************************************************/
#include "../Block/Block.h"
#include "Server/Server.h"
#include "Client/Client.h"
#include <vector>
#include <iostream>


/******************************************************************************
* CLASS BASE NODE
******************************************************************************/
class BaseNode
{

public:
	
	/*=====================================================
	* Name: getActionList
	* Entra: 
	* Resulta: 
	*=====================================================*/
	virtual std::vector<std::string> getActionList() = 0;

	/*=====================================================
	* Name: createBlockchainFromFile
	* Entra: String con la direcciion del archivo .json
	* Resulta: true si se pudo leer el archivo y false en caso contrario
	*=====================================================*/
	virtual bool createBlockchainFromFile(std::string&) = 0;

	/*=====================================================
	* Name: deleteBlockchain
	* Entra: -
	* Resulta: limpia la blockchain que este cargada
	*=====================================================*/
	virtual void deleteBlockchain();

	/*=====================================================
	* Name: getBlock
	* Entra: string con ID del bloque que se desea
	* Resulta: puntero al blocke solicitado
	*=====================================================*/
	virtual Block * getBlock(std::string id);

	/*=====================================================
	* Name: getBlocksID
	* Entra: vector de strings donde se guardaran las IDs. numero de bloques que 
	* se desean y offset referenciado al primer bloque. Si no se recibe numero de bloques se cargaran todos los disponibles
	* Resulta: true o false dependiendo si se pudieron leer todos los bloques o solo algunos.
	* Si la cantidad solicitada es mayor a la disponible se devuelven los que esten.
	*=====================================================*/
	virtual bool getBlocksID(std::vector<std::string>& buffer, int numOfBlocks = 0, int offset = 0);

	/*=====================================================
	* Name: getBlockQuant
	* Entra: -
	* Resulta: Numero de bloques cargados
	*=====================================================*/
	virtual int getBlockQuant(void);

	/*=====================================================
	* Name: getNodeType
	* Entra: -
	* Resulta: -
	*=====================================================*/
	virtual std::string getNodeType(void) = 0;


	bool addNeighbour(std::string ipAndPort, std::string nodeType);

	/*=====================================================
	* Name: addNeighbour
	* Entra: -
	* Resulta: 
	*=====================================================*/
	virtual bool addNeighbour(std::string ipAndPort, std::string nodeType) = 0;

	virtual std::string getIP();
	virtual std::string getPort();
	virtual std::string getState();
	virtual std::vector <std::pair<std::string, std::string>>& getNeighbours();
	virtual bool hasNeighbours();

	virtual bool poll();

protected:

	std::vector <std::pair<std::string, std::string>> neighbours;

	BaseNode(boost::asio::io_context& ioContext, boost::function<std::string(std::string, std::string)> msgReceivedCb, int portNum);

	/*=====================================================
	* Name: commSend used for POST messages
	* Entra: -
	* Resulta: envia un mensaje al nodo conectado.
	*=====================================================*/
	virtual void commSend(std::string host, std::string path, std::string& msg);

	/*=====================================================
	* Name: commSend used for GET messages
	* Entra: -
	* Resulta: envia un mensaje al nodo conectado.
	*=====================================================*/
	virtual void commSend(std::string host, std::string path);

	Client client;
	Server server;
	std::vector <Block> blockchain;		//blockchain con los datos
	int state;	//Estado del nodo (recibió mensaje, manda mensaje, escucha, etc.)
	std::vector <std::string> IPsSentList;
};
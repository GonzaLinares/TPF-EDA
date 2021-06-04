#pragma once
/***************************************************************************//**
@file     +FullNode.h+
@brief    +Header hijo+
@author   +Grupo 9+
******************************************************************************/


/******************************************************************************
* INCLUDE HEADER FILES
******************************************************************************/
#include "Block.h"
#include "BaseNode.h"
#include <vector>

/******************************************************************************
* TODO
******************************************************************************/

std::vector<std::string> FullNode::actionsVector = { "ACA VAN LOS NOMBRES DE LAS ACCIONES" };

/******************************************************************************
* CLASS NODE
******************************************************************************/
class FullNode : public BaseNode
{

public:

	// Inherited via BaseNode
	// override specifier ensures that the function is virtual and is overriding a virtual function from a base class

	virtual void commSend(void) override;

	virtual void commReceive(void) override;

	virtual Block * getBlock(std::string id) override;

	virtual bool getBlocksID(std::vector<std::string>& buffer, int numOfBlocks = 0, int offset = 0) override;

	virtual int getBlockQuant(void) override;

	virtual bool createBlockchainFromFile(std::string&) override;

	virtual void deleteBlockchain() override;


private:
	
	static std::vector<std::string> actionsVector;    //Vector con los nombres de las acciones posibles
};


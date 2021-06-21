/***************************************************************************//**
@file     +Main.cpp+
@brief    +Archivo donde esta la funcion main +
@author   +Grupo 9+
******************************************************************************/

/******************************************************************************
* INCLUDE HEADER FILES
******************************************************************************/
#include "Node/FullNode.h"		// Todo: sacar estos dos de aca
#include "Node/SPVNode.h"
#include "ImGui/Gui.h"
#include "MerkleTree.h"
#include "Hashing.h"
#include "Node/NodeFactory.h"

#include <cryptopp/hex.h>
#include <cryptopp/sha.h>

using namespace std;

/******************************************************************************
* Funcion Main
******************************************************************************/
int main(int argc, char** argv) {

	Gui menu;
	boost::asio::io_context contexto;
	NodeFactory nodes(contexto);

	////NODOS PARA DEBUGGING
	nodes.createFullNode(std::to_string(25576));
	nodes.createSPVNode(std::to_string(25578));
	nodes.createFullNode(std::to_string(25582) ,std::string("blockchain_sample_0.json"));
	
	nodes.getNodes()[2]->addNeighbour("127.0.0.1:25576", std::string("Full"));
	nodes.getNodes()[1]->addNeighbour("127.0.0.1:25582", std::string("Full"));
	////******************************************************
	
	while (menu.getState() == RUNNING) {

		nodes.pollAllNodes();
		menu.update(nodes);
	}
	

	return 0;
}
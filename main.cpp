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

using namespace std;

/******************************************************************************
* Funcion Main
******************************************************************************/
int main(int argc, char** argv) {

	Gui menu;
	boost::asio::io_context contexto;
	NodeFactory nodes(contexto);

	
	nodes.createFullNode(std::to_string(12344) ,std::string("blockchain_sample_0.json"));
	nodes.createFullNode(std::to_string(12366));
	nodes.getNodes()[0]->addNeighbour("127.0.0.1:12366", std::string("Full"));
	
	
	while (menu.getState() == RUNNING) {

		nodes.pollAllNodes();
		menu.update(nodes);
	}
	

	return 0;
}
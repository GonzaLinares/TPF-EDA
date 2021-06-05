/***************************************************************************//**
@file     +Main.cpp+
@brief    +Archivo donde esta la funcion main +
@author   +Grupo 9+
******************************************************************************/

/******************************************************************************
* INCLUDE HEADER FILES
******************************************************************************/
#include "Node/FullNode.h"
#include "ImGui/Gui.h"
#include "MerkleTree.h"
#include "Hashing.h"

using namespace std;

/******************************************************************************
* Funcion Main
******************************************************************************/
int main(int argc, char** argv) {

	Gui menu;
	vector<BaseNode*> nodes;
	boost::asio::io_context contexto;
	FullNode pene(contexto, "25565");
	FullNode penelo(contexto, "24465");
	while (menu.getState() == RUNNING) {

		contexto.poll();
		menu.update(nodes);
	}

	return 0;
}
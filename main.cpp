/***************************************************************************//**
@file     +Main.cpp+
@brief    +Archivo donde esta la funcion main +
@author   +Grupo 9+
******************************************************************************/

/******************************************************************************
* INCLUDE HEADER FILES
******************************************************************************/
#include "Node.h"
#include "Gui.h"
#include "MerkleTree.h"
#include "Hashing.h"
#include <vector>

using namespace std;

/******************************************************************************
* Funcion Main
******************************************************************************/
int main(int argc, char** argv) {

	Gui menu;
	vector<BaseNode*> nodes;

	while (menu.getState() == RUNNING) {
		menu.update(nodes);
	}

	return 0;
}
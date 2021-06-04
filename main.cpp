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

/******************************************************************************
* Funcion Main
******************************************************************************/
int main(int argc, char** argv) {

	Gui menu;
	FullNode nodo;

	while (menu.getState() == RUNNING) {
		menu.update(nodo);
	}

	return 0;
}
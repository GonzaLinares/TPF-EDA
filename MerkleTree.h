#pragma once
/***************************************************************************//**
@file     +MerkleNode.h+
@brief    +Header+
@author   +Grupo 9+
******************************************************************************/


/******************************************************************************
* INCLUDE HEADER FILES
******************************************************************************/
#include <iostream>
#include "Block/Tx.h"
#include "MerkleNode.h"

/******************************************************************************
* CLASS TEMPLATE MERKLE TREE
******************************************************************************/
template <std::string(*hashFunc)(std::string&)> class MerkleTree
{

public:

	/*=====================================================
	* Name: Constructor por defecto
	* Entra: -
	* Resulta: -
	*=====================================================*/
    MerkleTree();

	/*=====================================================
	* Name: Constructor del merkleTree
	* Entra: Vector de strings con las IDs de las transacciones para armar el merkletree
	* Resulta: -
	*=====================================================*/
	MerkleTree(std::vector <std::string>& txIds);

	/*=====================================================
	* Name: destructor
	* Entra: -
	* Resulta: -
	*=====================================================*/
    ~MerkleTree();

	/*=====================================================
	* Name: getMerkleRoot
	* Entra: -
	* Resulta: Devuelve el hash del nodo raiz
	*=====================================================*/
	std::string getMerkleRoot(void);

	/*=====================================================
	* Name: getRootNode
	* Entra: -
	* Resulta: Devuelve el nodo raiz
	*=====================================================*/
    MerkleNode* getRootNode(void);

	/*=====================================================
	* Name: getMerklePath
	* Entra: El id de la transaccion de la cual se desea el merkle path
    y tambien el vector de strings donde se va a guardar el merkle path
	* Resulta: -
	*=====================================================*/
	void getMerklePath(std::string& txid, std::vector<std::string>& merklepath);

private:

    MerkleNode* root;
    std::vector<MerkleNode*> blocks;

};
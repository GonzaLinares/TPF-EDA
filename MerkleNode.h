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
#include <vector>

/******************************************************************************
* CLASS MERKLENODE
******************************************************************************/
class MerkleNode {

public:             

    /*=====================================================
    * Name: constructor 
    * Entra: String con el nombre del hash para construir
    * Resulta: -
    *=====================================================*/
    MerkleNode(std::string data);

    /*=====================================================
    * Name: getHash
    * Entra: -
    * Resulta: Devuelve el hash
    *=====================================================*/
    std::string getHash();

    /*=====================================================
    * Name: Left
    * Entra: -
    * Resulta: Un puntero al nodo izquierdo
    *=====================================================*/
    MerkleNode*& Left(void);

    /*=====================================================
    * Name: Right
    * Entra: -
    * Resulta: Un puntero al nodo derecho
    *=====================================================*/
    MerkleNode*& Right(void);

    /*=====================================================
    * Name: getMerklePath
    * Entra: El id de la transaccion de la cual se desea el merkle path
    y tambien el vector de strings donde se va a guardar el merkle path
    * Resulta: -
    *=====================================================*/
    void getMerklePath(std::string& txid, std::vector<std::string>& merklepath);

private:
    std::string hash;
    MerkleNode* left;
    MerkleNode* right;
};

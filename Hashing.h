#pragma once
/***************************************************************************//**
@file     +Hashing.h+
@brief    +Funcion para hasheo+
@author   +Grupo 9+
******************************************************************************/

/******************************************************************************
* INCLUDE HEADER FILES
******************************************************************************/
#include <string>
/*=====================================================
* Name: hash32
* Entra: string con el contenido a hashear
* Resulta: Devulve el string hasheado en formato HexCodedAscii
*=====================================================*/
std::string hash32(std::string& str);

std::string hexCodedAscii(unsigned int asd);

#define NUMBEROFZEROSCHALLENGE 10
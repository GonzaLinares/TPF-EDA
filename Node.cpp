#include "Node.h"

void Node::connect(void)
{
}

void Node::disconnect(void)
{
}

void Node::commSend(void)
{
}

void Node::commReceive(void)
{
}

//Devuelve un puntero al bloque del id pedido, en el caso de no encontrar ninguno, devuelve NULL
Block * Node::getBlock(std::string id)
{
	Block* salida = nullptr;
	for (std::vector<Block>::iterator it = blockchain.begin(); it != blockchain.end(); it++) {

		if ((it->getBlockId()) == id) {

			salida = &(*it);
		}
	}

	return salida;
}

std::vector<std::string> Node::getBlocksID(int numOfBlocks, int offset)	//TODO: Que verga pasaba si numOfBlocks es 0 
{
	std::vector <std::string> temp;

	std::vector<Block>::iterator it = blockchain.begin();

	it = it + offset;

	for (int i = 1; i <= numOfBlocks ; it++, i++) {

		temp.push_back(it->getBlockId());
	}

	return temp;
}

int Node::getBlockQuant(void)
{
	return blockchain.size();
}

void Node::createBlockchainFromFile(void)
{
}

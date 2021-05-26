#include "Node.h"
#include "Gui.h"
#include "MerkleTree.h"
#include "Hashing.h"
#include <vector>

int main(int argc, char** argv) {

	Gui menu;
	Node nodo;

	/*
	std::string path("json.json");
	nodo.createBlockchainFromFile(path);
	std::vector<std::string> buffer;
	nodo.getBlock(std::string("13878957"))->getTxsID(buffer);
	MerkleTree<hash> arbolito(buffer);
	buffer.clear();
	path = "6A808C27";
	arbolito.getMerklePath(path, buffer);
	*/

	while (menu.getState() == RUNNING) {
		menu.update(nodo);
	}

	return 0;
}
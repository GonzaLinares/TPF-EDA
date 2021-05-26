#include "Node.h"
#include "Gui.h"

int main(int argc, char** argv) {

	Gui menu;
	Node nodo;
	std::string path = "json.json";
	nodo.createBlockchainFromFile(path);

	menu.init();

	while (menu.getState()) {
		menu.update(nodo);
	}

	menu.destroy();

	return 0;
}
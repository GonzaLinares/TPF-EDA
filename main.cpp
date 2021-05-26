#include "Node.h"
#include "Gui.h"

int main(int argc, char** argv) {

	Gui menu;
	Node nodo;

	while (menu.getState() == RUNNING) {
		menu.update(nodo);
	}

	return 0;
}
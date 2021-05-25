#include "Node.h"
#include "Gui.h"

int main(int argc, char** argv) {

	Gui menu;
	Node nodo;

	menu.init();

	while (menu.getState()) {
		menu.update();
	}

	menu.destroy();

	return 0;
}
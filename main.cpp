#include "Node.h"
#include "Gui.h"

int main(int argc, char** argv) {

	Gui menu;
	Node nodo;

	menu.init();

	menu.run();

	menu.destroy();

	return 0;
}
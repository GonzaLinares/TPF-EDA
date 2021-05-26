#pragma once

#include <string>
#include <iostream>
#include "Node.h"

enum STATES {CLOSEPROGRAMM};

class Gui
{
	public:

		void init();
		void update(Node& node);
		void destroy();
	
		int getState();

	private:

		int state;
		std::string filename;

		void showBlocksTab(Node& node);
		void openSubTreeNode(int n, int q);
};


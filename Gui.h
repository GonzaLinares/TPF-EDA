#pragma once

enum STATES {CLOSEPROGRAMM};

class Gui
{
	public:

		void init();
		void update();
		void destroy();
	
		int getState();

	private:

		int state;

		void showBlocksTab();
		void openSubTreeNode(int n, int q);
};


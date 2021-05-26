#pragma once

#include <string>
#include <iostream>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>      
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h> 
#include "Node.h"

enum STATES {CLOSEPROGRAM, RUNNING};

class Gui
{
	public:

		Gui();
		~Gui();

		void update(Node& node);
	
		int getState();

	private:

		int state;
		int blockPage;
		std::string filename;
		ALLEGRO_DISPLAY* display;
		ALLEGRO_EVENT_QUEUE* queue;

		void showBlocksTab(Node& node);
		void openSubTreeNode(int n, int q);
};


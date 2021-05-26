#pragma once

#include <string>
#include <iostream>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>      
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <map>
#include "Node.h"
#include "Hashing.h"
#include "MerkleTree.h"

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
		std::map<std::string, MerkleTree<hash32>*> merkleTrees;

		void deleteMerkleDic(std::map<std::string, MerkleTree<hash32>*>& dic);
		void showBlocksTab(Node& node);
		void openSubTreeNode(MerkleNode* node, int& id);

};


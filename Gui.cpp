#include "Gui.h"
#include "config.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_allegro5.h"
#include "ImGui/imgui_stdlib.h"
#include <stdexcept> 

using namespace std;

Gui::Gui() {
	
	state = RUNNING;
	blockPage = 0;
	filename = "";
	merkleTrees.clear();

	//Inicializamos allegro y sus principales addons
	if (!al_init()) {
		throw exception("Error al inicializar allegro");
	}

	if (!al_install_mouse()) {
		throw exception("Error al inicializar el mouse");
	}

	if (!al_install_keyboard()) {
		throw exception("Error al inicializar el teclado");
	}

	if (!al_init_primitives_addon()) {
		throw exception("Error al inicializar las primitivas de allegro");
	}

	if (!al_init_font_addon()) {
		throw exception("Error al inicializar font addon de allegro");
	}

	if (!al_init_image_addon()) {
		throw exception("Error al inicializar image addon de allegro");
	}

	if (!al_init_ttf_addon()) {
		throw exception("Error al inicializar ttf addon de allegro");
	}

	//Creamos un display
	display = al_create_display(SCREENWIDTH, SCREENHEIGHT);
	if (display == nullptr)
	{
		throw exception("Error al crear la ventana");
	}

	queue = al_create_event_queue();
	if (queue == nullptr)
	{
		throw exception("Error al crear la cola de eventos");
	}

	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_mouse_event_source());

	al_set_window_title(display, "Blockchain");

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplAllegro5_Init(display);
}

Gui::~Gui()
{
	// Cleanup
	ImGui_ImplAllegro5_Shutdown();
	ImGui::DestroyContext();
	al_destroy_event_queue(queue);
	al_destroy_display(display);
}

void Gui::update(Node& node) {

	ALLEGRO_EVENT ev;
	const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGuiWindowFlags window_flags = 0;
	ImGuiCond window_cond = 0;
	ImGuiTabBarFlags tab_bar_flags = 0;

	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	//window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	//window_cond |= ImGuiCond_FirstUseEver;
	tab_bar_flags |= ImGuiTabBarFlags_None;

    while (al_get_next_event(queue, &ev))
    {
        ImGui_ImplAllegro5_ProcessEvent(&ev);
		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			deleteMerkleDic(merkleTrees);
			state = CLOSEPROGRAM;
		}
    }

    // Start the Dear ImGui frame
    ImGui_ImplAllegro5_NewFrame();
    ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(0, 0), window_cond);
	ImGui::SetNextWindowSize(ImVec2(SCREENWIDTH, SCREENHEIGHT), window_cond);
	ImGui::Begin("Blockchain Explorer", NULL, window_flags);

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("Choose a file");
	ImGui::Spacing();
	ImGui::InputText(" ", &filename);
	ImGui::SameLine();
	if (ImGui::Button("Load file")) {
		deleteMerkleDic(merkleTrees);
		merkleTrees.clear();
		node.createBlockchainFromFile(filename);
	}

	ImGui::Spacing();
	ImGui::Spacing();

	if (ImGui::BeginTabBar("Options", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Blocks"))
		{
			showBlocksTab(node);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Coming Soon"))
		{
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
    ImGui::End();

    // Rendering
    ImGui::Render();
	al_clear_to_color(al_map_rgba_f(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w));
    ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
    al_flip_display();

}

int Gui::getState() {
	return state;
}

void Gui::deleteMerkleDic(std::map<std::string, MerkleTree<hash32>*>& dic)
{

	for (auto it : dic)
	{
		if (it.second != nullptr)
		{
			delete it.second;
		}
		
	}

}

void Gui::showBlocksTab(Node& node) {

	const int cols = 2;
	const int pageSize = cols * 2;
	int label = 0;
	bool checkbox = false;
	int blocksQuant = 0;
	string aux;

	std::vector<std::string> currentPageBlockIDs;
	node.getBlocksID(currentPageBlockIDs, pageSize, blockPage);
	blocksQuant = currentPageBlockIDs.size();

	ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
	ImGui::Columns(cols, NULL, true);
	ImGui::SetColumnWidth(0, SCREENWIDTH/cols);

	for (int i = 0; i < blocksQuant; i++){

		checkbox = false;
		Block* currentBlock = node.getBlock(currentPageBlockIDs[i]);
		string currentID = currentBlock->getId();

		if (ImGui::GetColumnIndex() == 0) {
			ImGui::Separator();
		}

		ImGui::Text("Block %d", 1+i+blockPage);
		ImGui::Spacing();
		ImGui::Text("current block hash: %s", currentID.c_str());
		ImGui::Text("previous block hash: %s", currentBlock->getPrevBlockId().c_str());
		ImGui::Text("number of transactions: %d", currentBlock->getnTx());
		ImGui::Text("nonce: %d", currentBlock->getNonce());
		ImGui::Text("merkle root: %s", currentBlock->getMerkleRoot().c_str());
		aux = "calculate merkle##" + currentID;
		
		if (ImGui::Button(aux.c_str())) {
			std::vector<std::string> currentTrxs;
			currentBlock->getTxsID(currentTrxs);
			merkleTrees.insert(pair<string, MerkleTree<hash32>*>(currentID, new MerkleTree<hash32>(currentTrxs)));
		}
		
		std::map<string, MerkleTree<hash32>*>::iterator merkleIt = merkleTrees.find(currentID);
		if (merkleIt != merkleTrees.end()) {

			ImGui::Text("calculated: %s", (*merkleIt).second->getMerkleRoot().c_str());
			if (merkleTrees.count(currentID) > 0 && currentBlock->getMerkleRoot() == (*merkleIt).second->getMerkleRoot()) {
				checkbox = true;
			}
			ImGui::SameLine();
			aux = "##" + currentID;
			ImGui::Checkbox(aux.c_str(), &checkbox);

			if (ImGui::TreeNode((const void*)(++label), "merkle tree", i))
			{
				MerkleNode* merkleNodeRoot = (*merkleIt).second->getRootNode();

				if (ImGui::TreeNode((const void*)(++label), "root")) {
					ImGui::Text(merkleNodeRoot->getHash().c_str());

					openSubTreeNode(merkleNodeRoot, label);

					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
		}
		else {
			ImGui::Text("calculated: ???");
		}
		

		ImGui::Spacing();

		ImGui::NextColumn();
	}

	ImGui::Columns(1);

	if (blocksQuant > 0) {
		ImGui::SetCursorPos(ImVec2(SCREENWIDTH - 300, 20));
		if (ImGui::Button("Prev", ImVec2(100, 50))) {
			if (pageSize <= blockPage) {
				blockPage -= pageSize;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Next", ImVec2(100, 50))) {
			if (blockPage + pageSize < node.getBlockQuant()) {
				blockPage += pageSize;
			}
		}
	}
}

void Gui::openSubTreeNode(MerkleNode* node, int& id) {

	if (node->Left() != nullptr && ImGui::TreeNode((const void*)(++id), "left")) {
		ImGui::Text(node->Left()->getHash().c_str());
		openSubTreeNode(node->Left(), id);
		ImGui::TreePop();
	}
	if (node->Right() != nullptr && ImGui::TreeNode((const void*)(++id), "right")) {
		ImGui::Text(node->Right()->getHash().c_str());
		openSubTreeNode(node->Right(), id);
		ImGui::TreePop();
	}
	
}
#include "Gui.h"
#include "config.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_allegro5.h"
#include "ImGui/imgui_stdlib.h"
#include <stdexcept> 
#include <filesystem>

using namespace std;

Gui::Gui() {
	
	state = RUNNING;
	blockPage = 0;
	filename = "";

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

void Gui::showBlocksTab(Node& node) {

	int openedNodes = 0;
	const int cols = 2;
	const int rows = 10;
	bool checkbox = true;
	int blocksQuant = 0;
	int pageSize = cols*2;

	ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
	ImGui::Columns(cols, NULL, true);
	ImGui::SetColumnWidth(0, SCREENWIDTH/cols);

	std::vector<std::string> blockIDs;
	node.getBlocksID(blockIDs, pageSize, blockPage);
	blocksQuant = blockIDs.size();

	for (int i = 0; i < blocksQuant; i++){

		Block* currentBlock = node.getBlock(blockIDs[i]);

		if (ImGui::GetColumnIndex() == 0) {
			ImGui::Separator();
		}

		ImGui::Text("Block %d", i+1);
		ImGui::Spacing();
		ImGui::Text("current block hash: %s", currentBlock->getId().c_str());
		ImGui::Text("previous block hash: %s", currentBlock->getPrevBlockId().c_str());
		ImGui::Text("number of transactions: %d", currentBlock->getnTx());
		ImGui::Text("nonce: %d", currentBlock->getNonce());
		ImGui::Text("merkle root: %s", currentBlock->getMerkleRoot().c_str());
		if (ImGui::Button("calculate merkle")) {

		}
		ImGui::Text("calculated: %s", "d52c9f6fd4ea571ae30cd0973fa2a4fac282888cda19ecc20f6919bcf49fcbf0");
		ImGui::SameLine();
		ImGui::Checkbox("", &checkbox);

		if (ImGui::TreeNode((void*)(intptr_t)(0), "merkle tree"))
		{
			if (ImGui::TreeNode((void*)(intptr_t)(1), "root")) {
				ImGui::Text("d52c9f6fd4ea571ae30cd0973fa2a4fac282888cda19ecc20f6919bcf49fcbf0");

				openSubTreeNode(4, openedNodes);

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		ImGui::Spacing();

		//ImGui::Text("Width %.2f", ImGui::GetColumnWidth());
		//ImGui::Text("Avail %.2f", ImGui::GetContentRegionAvail().x);
		//ImGui::Text("Offset %.2f", ImGui::GetColumnOffset());
		//ImGui::Text("Long text that is likely to clip");
		//ImGui::Button("Button", ImVec2(-FLT_MIN, 0.0f));
		
		ImGui::NextColumn();
	}

	ImGui::Columns(1);

	if (ImGui::Button("Prev")) {
		if (pageSize < blockPage) {
			blockPage -= pageSize;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Next")) {
		if (blocksQuant/pageSize < blockPage) {
			blockPage += pageSize;
		}
	}
}

void Gui::openSubTreeNode(int n, int q) {

	if (n == 0) {
		if (ImGui::TreeNode((void*)(intptr_t)(q), "leaf %d", n)) {
			ImGui::Text("d52c9f6fd4ea571ae30cd0973fa2a4fac282888cda19ecc20f6919bcf49fcbf0");
			ImGui::TreePop();
		}
	}
	else {
		if (ImGui::TreeNode((void*)(intptr_t)(q), "right %d", n)) {
			openSubTreeNode(n - 1, q + 1);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode((void*)(intptr_t)(-q), "left %d", n)) {
			openSubTreeNode(n - 1, q - 1);
			ImGui::TreePop();
		}
	}
}
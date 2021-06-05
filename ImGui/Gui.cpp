#include "Gui.h"
#include "../config.h"
#include "imgui.h"
#include "imgui_impl_allegro5.h"
#include "imgui_stdlib.h"
#include <stdexcept> 

using namespace std;

enum NodeTableColumns {TYPE_TABLE_IDENTIFIER, IP_TABLE_IDENTIFIER, PORT_TABLE_IDENTIFIER, CONNECTIONS_TABLE_IDENTIFIER};

Gui::Gui() {
	
	state = RUNNING;
	blockPage = 0;
	filename = "";
	fileFounded = true;
	linkedSuccess = true;
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

	al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
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

void Gui::update(vector<BaseNode*>& nodes) {

	ALLEGRO_EVENT ev;
	const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);	//Color de fondo de pantalla

	ImGuiWindowFlags window_flags = 0;		//Flags de configuracion del GUI
	ImGuiCond window_cond = 0;
	ImGuiTabBarFlags tab_bar_flags = 0;

	window_flags |= ImGuiWindowFlags_NoTitleBar;		//Configuraciones del la GUI
	window_flags |= ImGuiWindowFlags_NoMove;
	//window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	//window_cond |= ImGuiCond_FirstUseEver;
	tab_bar_flags |= ImGuiTabBarFlags_None;

    while (al_get_next_event(queue, &ev))		//Leemos los eventos de allegro
    {
        ImGui_ImplAllegro5_ProcessEvent(&ev);
		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			deleteMerkleDic();
			merkleTrees.clear();
			state = CLOSEPROGRAM;
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_RESIZE)
		{
			ImGui_ImplAllegro5_InvalidateDeviceObjects();
			al_acknowledge_resize(display);
			ImGui_ImplAllegro5_CreateDeviceObjects();
		}
    }

    // Start the Dear ImGui frame
    ImGui_ImplAllegro5_NewFrame();
    ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(0, 0), window_cond);
	ImGui::SetNextWindowSize(ImVec2(SCREENWIDTH, SCREENHEIGHT), window_cond);	//Creamos la ventana
	ImGui::Begin("Blockchain Explorer", NULL, window_flags);
	
	nSpacing(4);

	ImGui::Text("Welcome to the blockchain explorer made by Agustin Gullino, Damian Sergi and Gonzalo Linares");
	
	nSpacing(4);

	if (ImGui::BeginTabBar("Options", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Nodes"))		//Tab para completar
		{
			showNodesTab(nodes);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Blocks 8090"))	//Tab de los bloques
		{
			//showBlocksTab(nodes[0]);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("DemoWindows"))	//Tab de los bloques
		{
			bool show_demo_window = true;
			ImGui::ShowDemoWindow(&show_demo_window);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
    ImGui::End();

    // Rendering
    ImGui::Render();	//Actualizamos el display
	al_clear_to_color(al_map_rgba_f(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w));	
    ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
    al_flip_display();

}

int Gui::getState() {
	return state;
}

void Gui::deleteMerkleDic()
{
	for (auto it : merkleTrees)	
	{
		if (it.second != nullptr)
		{
			delete it.second;
		}
	}
}

void Gui::showNodesTab(vector<BaseNode*>& nodes) {

	const ImVec2 center = ImGui::GetMainViewport()->GetCenter();

	static string nodeIp = "";
	static string nodePort = "";

	static string connPort1 = "";
	static string connPort2 = "";

	static string senderNode = "";
	static string receiverNode = "";
	static string message = "";

	static string publicKey = "";
	static int coinAmount = 0;

	const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
	static int item_current_idx = 0; // Here we store our selection data as an index.
	const char* combo_label = items[item_current_idx];  // Label to preview before opening the combo (technically it could be anything)

	static ImGuiTableFlags flags =
		ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
		| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter  | ImGuiTableFlags_NoBordersInBody
		| ImGuiTableFlags_ScrollY;

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f); //TODO: Ver porque no aparecen los bordes redondeados
	
	nSpacing(3);
	ImGui::BeginChild("CreateNode", ImVec2(300, 130), true);
	ImGui::Text("1) Create node");
	nSpacing(3);
	ImGui::Text("IP:Port");
	ImGui::SameLine();
	ImGui::InputText("###IP:Port001", &nodeIp);
	ImGui::Text("Blockchain file");
	ImGui::SameLine(); 
	ImGui::InputText(" ", &filename);
	/*
	ImGui::SameLine();
	if (ImGui::Button("Load file")) {		//Cargamos el archivo solicitado
		deleteMerkleDic();
		merkleTrees.clear();
		nodes[0]->deleteBlockchain();
		fileFounded = nodes[0]->createBlockchainFromFile(filename);
		blockPage = 0;
	}
	*/

	if (fileFounded == false) {		//Si no encontro el archivo mostramos el mensaje de error
		nSpacing(1);
		ImGui::Text("File not found!!");
	}

	nSpacing(2);

	ImGui::Button("SPV");
	ImGui::SameLine();
	ImGui::Button("Full");
	ImGui::EndChild();


	ImGui::BeginChild("Connect", ImVec2(300, 130), true);
	ImGui::Text("2) Connect");
	nSpacing(3);
	ImGui::Text("IP:Port");
	ImGui::SameLine();
	ImGui::InputText("###IP:Port002", &connPort1);
	nSpacing(1);
	if (ImGui::Button("Link")) {
		cout << connPort1 << " and " << connPort2 << " linked" << endl;
		linkedSuccess = false;
	}
	nSpacing(4);
	if (!linkedSuccess) {
		ImGui::Text("Mustn't be linked two SPV nodes");
	}
	ImGui::EndChild();


	ImGui::SetCursorPos(ImVec2(320,80));
	ImGui::BeginChild("NodeList", ImVec2(480, 300), true);
	if (ImGui::BeginTable("table_sorting", 6, flags))
	{
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_PreferSortDescending | ImGuiTableColumnFlags_WidthFixed, 0.0f, TYPE_TABLE_IDENTIFIER);
		ImGui::TableSetupColumn("IP", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, IP_TABLE_IDENTIFIER);
		ImGui::TableSetupColumn("Port", ImGuiTableColumnFlags_WidthFixed, 0.0f, PORT_TABLE_IDENTIFIER);
		ImGui::TableSetupColumn("Active", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, CONNECTIONS_TABLE_IDENTIFIER);
		ImGui::TableSetupColumn("Connections", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, CONNECTIONS_TABLE_IDENTIFIER);
		ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, CONNECTIONS_TABLE_IDENTIFIER);
		ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
		ImGui::TableHeadersRow();

		ImGuiListClipper clipper;
		//clipper.Begin(items.Size);
		clipper.Begin(20);
		while (clipper.Step())
			//cout << clipper.DisplayStart << endl;
			for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
			{
				static int e = 0;
				// Display a data item
				//MyItem* item = &items[row_n];
				//ImGui::PushID(item->ID);
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("%s", "SPV");
				ImGui::TableNextColumn();
				ImGui::Text("%s", "134.56.235.7");
				ImGui::TableNextColumn();
				ImGui::Text("%s", "8080");
				ImGui::TableNextColumn();
				ImGui::RadioButton("###radio a", &e); e++;
				ImGui::TableNextColumn();
				if (true) { //Si tiene conexiones
					char buffer[32];
					sprintf_s(buffer, "Show###%d", row_n);
					if (ImGui::SmallButton(buffer)) {
						ImGui::OpenPopup(buffer);
					}
					
					sprintf_s(buffer, "Connections###%d", row_n);
					ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
					if (ImGui::BeginPopupModal(buffer, NULL, ImGuiWindowFlags_AlwaysAutoResize))
					{
						ImGui::Button("X");
						ImGui::SameLine();
						ImGui::Text("IP: %s", "AAAAA");
						
						if (ImGui::Button("Close", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
						ImGui::SetItemDefaultFocus();
						ImGui::EndPopup();
					}
				}
				else {
					ImGui::Text("%s", "Empty");
				}
				ImGui::TableNextColumn();
				ImGui::Text("Receiving");
				//ImGui::PopID();
			}
		ImGui::EndTable();
	}
	ImGui::EndChild();

	
	ImGui::SetCursorPos(ImVec2(0, 400));
	ImGui::BeginChild("MessageTab", ImVec2(0, 180), true);

	ImGui::Text("3) Actions");
	nSpacing(3);
	ImGui::Text("Receiver IP:Port");
	ImGui::SameLine();	
	if (ImGui::BeginCombo("###Receiver IP:Port", combo_label))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{
			const bool is_selected = (item_current_idx == n);
			if (ImGui::Selectable(items[n], is_selected))
				item_current_idx = n;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::Text("Message:");
	ImGui::SameLine();
	if (ImGui::BeginCombo("###Message:", combo_label))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{
			const bool is_selected = (item_current_idx == n);
			if (ImGui::Selectable(items[n], is_selected))
				item_current_idx = n;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (true) { //Message send crypto == true
		ImGui::Text("Public key: ");
		ImGui::SameLine();
		ImGui::InputText("###Public key :", &publicKey);
		ImGui::SameLine();
		ImGui::Text("Amount: ");
		ImGui::SameLine();
		ImGui::InputInt("###Amount:", &coinAmount);
		nSpacing(5);
		ImGui::Button("Send Message");
	}

	ImGui::EndChild();
	



	ImGui::PopStyleVar();

}

static void PushStyleCompact()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, (float)(int)(style.FramePadding.y * 0.60f)));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x, (float)(int)(style.ItemSpacing.y * 0.60f)));
}

static void PopStyleCompact()
{
	ImGui::PopStyleVar(2);
}

void Gui::showBlocksTab(BaseNode& node) {

	const int cols = 2;
	const int pageSize = cols * 2;	//Tamanio de la pagina
	int label = 0;
	bool merkleRootOk = false;
	int blocksQuant = 0;
	string aux;

	std::vector<std::string> currentPageBlockIDs;	//Cargamos los ids de los bloques que se van a mostrar en la pagina actual
	node.getBlocksID(currentPageBlockIDs, pageSize, blockPage);
	blocksQuant = currentPageBlockIDs.size();

	ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);	//Configuramos las columnas
	ImGui::Columns(cols, NULL, true);
	ImGui::SetColumnWidth(0, SCREENWIDTH/cols);

	for (int i = 0; i < blocksQuant; i++){

		merkleRootOk = false;	//Indica si el merkle root calculado coincide con el que tenia el bloque en el json
		Block* currentBlock = node.getBlock(currentPageBlockIDs[i]);	//Obtenemos el bloque actual en base al blockID
		string currentID = currentBlock->getId();

		if (ImGui::GetColumnIndex() == 0) {	
			ImGui::Separator();
		}

		//Mostramos la informacion del bloque
		ImGui::Text("Block %d", 1+i+blockPage);
		nSpacing(1);
		ImGui::Text("Current block hash: %s", currentID.c_str());
		ImGui::Text("Previous block hash: %s", currentBlock->getPrevBlockId().c_str());
		ImGui::Text("Number of transactions: %d", currentBlock->getnTx());
		ImGui::Text("Nonce: %d", currentBlock->getNonce());
		ImGui::Text("Merkle root: %s", currentBlock->getMerkleRoot().c_str());
		aux = "Calculate Merkle Tree##" + currentID;
		
		//Si se presiono el boton de calcular el merkle root
		if (ImGui::Button(aux.c_str())) {
			std::vector<std::string> currentTrxs;	//Creamos el merkle root en base a las transacciones que tiene el bloque
			currentBlock->getTxsID(currentTrxs);
			merkleTrees.insert(pair<string, MerkleTree<hash32>*>(currentID, new MerkleTree<hash32>(currentTrxs)));
		}
		
		std::map<string, MerkleTree<hash32>*>::iterator merkleIt = merkleTrees.find(currentID);	//Buscamos el par id del bloque/merkle tree
		if (merkleIt != merkleTrees.end()) {

			//Tilde que indica si esta bien calculado en merkle tree
			ImGui::Text("Calculated: %s", (*merkleIt).second->getMerkleRoot().c_str());
			if (merkleTrees.count(currentID) > 0 && currentBlock->getMerkleRoot() == (*merkleIt).second->getMerkleRoot()) {	//Si el merkle tree fue creado y conicide el id calculado con el que tiene el bloque
				merkleRootOk = true;
			}
			ImGui::SameLine();
			aux = "##" + currentID;
			ImGui::Checkbox(aux.c_str(), &merkleRootOk);

			//Arbol de pestanias que muestran el merkle tree
			if (ImGui::TreeNode((const void*)(++label), "Merkle Tree", i))
			{
				MerkleNode* merkleNodeRoot = (*merkleIt).second->getRootNode();

				//Mostramos el arbol
				if (ImGui::TreeNode((const void*)(++label), "Root:")) {
					ImGui::SameLine();
					ImGui::Text(merkleNodeRoot->getHash().c_str());

					openSubTreeNode(merkleNodeRoot, label);

					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
		}
		else {
			ImGui::Text("Calculated: Press Calculate Merkle Tree");
		}
		
		nSpacing(1);

		ImGui::NextColumn();
	}

	ImGui::Columns(1);

	//Verificamos que se pueda avanzar o retroceder la pagina que muestra los bloques
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

	if (node->Left() != nullptr && ImGui::TreeNode((const void*)(++id), "Left:")) {
		ImGui::SameLine();
		ImGui::Text(node->Left()->getHash().c_str());
		openSubTreeNode(node->Left(), id);
		ImGui::TreePop();
	}
	if (node->Right() != nullptr && ImGui::TreeNode((const void*)(++id), "Right:")) {
		ImGui::SameLine();
		ImGui::Text(node->Right()->getHash().c_str());
		openSubTreeNode(node->Right(), id);
		ImGui::TreePop();
	}
	
}

void Gui::nSpacing(int n) {
	for (int i = 0; i < n; i++) {
		ImGui::Spacing();
	}
}
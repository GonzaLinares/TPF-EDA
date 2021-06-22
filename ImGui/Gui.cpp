#include "Gui.h"
#include "../config.h"
#include "imgui.h"
#include "imgui_impl_allegro5.h"
#include "imgui_stdlib.h"
#include <math.h>
#include <stdexcept> 

using namespace std;

enum NodeTableColumns {TYPE_TABLE_IDENTIFIER, IP_TABLE_IDENTIFIER, PORT_TABLE_IDENTIFIER, CONNECTIONS_TABLE_IDENTIFIER};

Gui::Gui() {
	
	state = RUNNING;
	blockPage = 0;
	currentNodeActive = 0;
	comboBoxNodesIndex = 0;
	comboBoxVerifiedTxsIndex = 0;
	comboBoxActionNodesIndex = 0;
	filename = "";
	nodeIp = "";
	nodePort = "";
	connPort1 = "";
	fileFound = true;
	linkedSuccess = true;
	verifiedTrxSendActive = false;
	falseActionActive = false;
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

void Gui::update(NodeFactory& nodes) {

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
		if (nodes.getNodes().size() > 0 && ImGui::BeginTabItem("Blocks"))	//Tab de los bloques
		{
			showBlocksTab(*nodes.getNodes()[currentNodeActive]);
			ImGui::EndTabItem();
		}
		/*
		if (ImGui::BeginTabItem("DemoWindows"))	//Tab de los bloques
		{
			bool show_demo_window = true;
			ImGui::ShowDemoWindow(&show_demo_window);
			ImGui::EndTabItem();
		}
		*/
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

void Gui::showNodesTab(NodeFactory& nodes) {

	showCreateBox(nodes);
	if (nodes.getNodes().size() > 0) {
		showConnectBox(nodes);
	}
	showNodesTable(nodes);
	if (nodes.getNodes().size() > 0) {
		showActionsBox(nodes);
		
	}
}

void Gui::showCreateBox(NodeFactory& nodes) {

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);

	nSpacing(3);
	ImGui::BeginChild("CreateNode", ImVec2(300, 150), true);
	ImGui::Text("1) Create node");
	nSpacing(3);
	ImGui::Text("Port");
	ImGui::SameLine();
	ImGui::InputText("###Port001", &nodePort);
	ImGui::Text("Blockchain file");
	ImGui::SameLine();
	ImGui::InputText("", &filename);  //Ingresamos el archivo donde cargamos un nodo
	nSpacing(2);
	if (ImGui::Button("SPV")) {		//Si se presiono SPV creamos un SPV con los datos ingresados...

		if (!nodes.alreadyExist(nodePort))
		{
			if (filename.size() > 0) {
				fileFound = nodes.createSPVNode(nodePort, filename);
			}
			else {
				nodes.createSPVNode(nodePort);
			}
			blockPage = 0;
			comboBoxNodesIndex = 0;
			comboBoxActionNodesIndex = 0;
			 
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Full")) {		//Sino creamos un Full
		if (!nodes.alreadyExist(nodePort))
		{
			if (filename.size() > 0) {
				fileFound = nodes.createFullNode(nodePort, filename);
			}
			else {
				nodes.createFullNode(nodePort);
			}
			blockPage = 0;
			comboBoxNodesIndex = 0;
			comboBoxActionNodesIndex = 0;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Miner")) {		//Sino creamos un Miner
		if (!nodes.alreadyExist(nodePort))
		{
			if (filename.size() > 0) {
				fileFound = nodes.createMinerNode(nodePort, filename);
			} 
			else {
				nodes.createMinerNode(nodePort);
			}
			blockPage = 0;
			comboBoxNodesIndex = 0;
			comboBoxActionNodesIndex = 0;
		}
	}

	if (fileFound == false) {		//Si no encontro el archivo mostramos el mensaje de error
		nSpacing(1);
		ImGui::Text("Blockchain not found!!");
	}

	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void Gui::showConnectBox(NodeFactory& nodes) {

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);

	ImGui::BeginChild("Connect", ImVec2(300, 150), true);
	ImGui::Text("2) Connect");
	nSpacing(3);
	ImGui::Text("IP:Port");
	ImGui::SameLine();
	ImGui::InputText("###IP:Port002", &connPort1);
	nSpacing(1);
	if (ImGui::Button("SPV")) {
		linkedSuccess = nodes.getNodes()[currentNodeActive]->addNeighbour(connPort1, "SPV");
	}
	ImGui::SameLine();
	if (ImGui::Button("Full")) {
		linkedSuccess = nodes.getNodes()[currentNodeActive]->addNeighbour(connPort1, "Full");
	}
	nSpacing(4);

	if (!linkedSuccess) {
		ImGui::Text("two SPV nodes can't be linked");
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void Gui::showActionsBox(NodeFactory& nodes) {

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);

	BaseNode* currentNode = nodes.getNodes()[currentNodeActive];
	vector <pair<string, string>> neigh = currentNode->getNeighbours();
	vector<string> actionList = currentNode->getActionList();
	pair<string, string> aux;
	vector<Tx> verifiedTxsList;

	if (currentNode->getVerifiedTxs().size() == 0) {
		verifiedTxsList.push_back(Tx(string("Empty")));
	}
	else {
		verifiedTxsList = currentNode->getVerifiedTxs();
	}

	if (neigh.empty()) {
		aux.first = "Empty";
		aux.second = "NULL";
		neigh.push_back(aux);
	}

	ImGui::SetCursorPos(ImVec2(9, 400));
	ImGui::BeginChild("MessageTab", ImVec2(0, 180), true);

	ImGui::Text("3) Actions");
	nSpacing(3);
	ImGui::Text("Receiver IP:Port");
	ImGui::SameLine();

	if (ImGui::BeginCombo("###Receiver IP:Port", neigh[comboBoxNodesIndex].first.c_str()))
	{
		for (unsigned int i = 0; i < neigh.size(); i++)
		{
			if (ImGui::Selectable(neigh[i].first.c_str(), comboBoxNodesIndex == i)) {
				comboBoxNodesIndex = i;
			}
			if (comboBoxNodesIndex == i) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::Text("Message:");
	ImGui::SameLine(); 
	ImGui::SetNextItemWidth(200.0f);
	if (ImGui::BeginCombo("###Message:", actionList[comboBoxActionNodesIndex].c_str()))
	{
		for (unsigned int i = 0; i < actionList.size(); i++)
		{
			const bool is_selected = (comboBoxActionNodesIndex == i);
			if (ImGui::Selectable(actionList[i].c_str(), is_selected))
				comboBoxActionNodesIndex = i;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}


	if (actionList[comboBoxActionNodesIndex] == std::string("TransactionPost") && neigh[comboBoxNodesIndex].second != "SPV") { //Message send crypto == true

		ImGui::SameLine();
		ImGui::Text("False Trx");
		ImGui::SameLine();
		if (ImGui::Checkbox("###falseActionActive", &falseActionActive)) {
			verifiedTrxSendActive = false;
		}
		ImGui::SameLine();
		ImGui::Text("Verified Trx");
		ImGui::SameLine();
		if (ImGui::Checkbox("###verifiedTrxSendActive", &verifiedTrxSendActive)) {
			falseActionActive = false;
		}
		 
		if (verifiedTrxSendActive) {

			ImGui::Text("Verified Trx: ");
			ImGui::SameLine();
			if (ImGui::BeginCombo("###Verified Trx", verifiedTxsList[comboBoxVerifiedTxsIndex].getId().c_str()))
			{
				for (unsigned int i = 0; i < verifiedTxsList.size(); i++)
				{ 
					if (ImGui::Selectable(verifiedTxsList[i].getId().c_str(), comboBoxVerifiedTxsIndex == i)) {
						comboBoxVerifiedTxsIndex = i;
					} 
					if (comboBoxVerifiedTxsIndex == i) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			} 
		}

		if (!verifiedTrxSendActive && !falseActionActive) {
			ImGui::Text("Public key: ");
			ImGui::SameLine();
			ImGui::InputText("###Public key: ", &actionGetBlockPublicKeyWritten);
			ImGui::SameLine();
			ImGui::Text("Amount: ");
			ImGui::SameLine();
			ImGui::InputInt("###Amount:", &amountWritten);
		}

		if (falseActionActive && currentNode->getNodeType() == "Full") {
			UTXO lastUTXO = ((FullNode*)currentNode)->getLastSpentUTXO();
			string aux = "";
			if (lastUTXO.getBlockId() != "Empty") {
				aux = "ID: " + lastUTXO.getBlockId() + "- Amount: " + to_string(lastUTXO.getAmount());
				ImGui::Text("Last UTXO Spent: ");
				ImGui::SameLine();
				ImGui::Text(aux.c_str());
			}
		}
		
		nSpacing(5);
		if (ImGui::Button("Send Message") && neigh[0].first != "Empty") {

			int lastNumOfTxs = currentNode->getVerifiedTxs().size();

			if (currentNode->getNodeType() == std::string("Full") ) {
				((FullNode*)currentNode)->transactionPost(actionGetBlockPublicKeyWritten, amountWritten, neigh[comboBoxNodesIndex].first);
			}
			else if (currentNode->getNodeType() == std::string("SPV")) {
				((SPVNode*)currentNode)->transactionPost(actionGetBlockPublicKeyWritten, amountWritten, neigh[comboBoxNodesIndex].first);
			}

			if (lastNumOfTxs == currentNode->getVerifiedTxs().size()) {
				cout << "Se envio una trx falsa" << endl;
			}
		}
	}
	else if (actionList[comboBoxActionNodesIndex] == std::string("BlockPost") && neigh[comboBoxNodesIndex].second != "SPV") {

		ImGui::Text("BlockID: ");
		ImGui::SameLine();
		ImGui::InputText("###BlockID :", &actionGetBlockIDWritten);

		nSpacing(5);
		if (ImGui::Button("Send Message") && neigh[0].first != "Empty") {
			((FullNode*)currentNode)->blockPost(neigh[comboBoxNodesIndex].first, actionGetBlockIDWritten);
		}
	}
	else if (actionList[comboBoxActionNodesIndex] == std::string("MerkleBlockPost") && neigh[comboBoxNodesIndex].second != "Full") {

		ImGui::Text("BlockID: ");
		ImGui::SameLine();
		ImGui::InputText("###BlockID :", &actionGetBlockIDWritten);
		ImGui::SameLine();
		ImGui::Text("Position: ");
		ImGui::SameLine();
		ImGui::InputInt("###Position:", &positionWritten);
		nSpacing(5);
		if (ImGui::Button("Send Message") && neigh[0].first != "Empty") {

			((FullNode*)currentNode)->merkleBlockPost(actionGetBlockIDWritten, positionWritten, neigh[comboBoxNodesIndex].first);
		}
	}
	else if (actionList[comboBoxActionNodesIndex] == std::string("GetBlocksPost") && neigh[comboBoxNodesIndex].second != "SPV") {

		ImGui::Text("BlockID: ");
		ImGui::SameLine();
		ImGui::InputText("###BlockID :", &actionGetBlockIDWritten);
		ImGui::SameLine();
		ImGui::Text("Blocks Quantity: ");
		ImGui::SameLine();
		ImGui::InputInt("###Blocks Quantity:", &blockQuantityWritten);
		nSpacing(5);
		if (ImGui::Button("Send Message") && neigh[0].first != "Empty") {

			((FullNode*)currentNode)->getBlocks(actionGetBlockIDWritten, std::to_string(blockQuantityWritten), neigh[comboBoxNodesIndex].first);
		}
	}
	else if (actionList[comboBoxActionNodesIndex] == std::string("FilterPost") && neigh[comboBoxNodesIndex].second != "SPV") {

		nSpacing(5);
		if (ImGui::Button("Send Message") && neigh[0].first != "Empty") {
			((SPVNode*)currentNode)->filterPost(neigh[comboBoxNodesIndex].first);
		}
	}
	else if (actionList[comboBoxActionNodesIndex] == std::string("GetBlockHeaders") && neigh[comboBoxNodesIndex].second != "SPV") {

		ImGui::Text("BlockID: ");
		ImGui::SameLine();
		ImGui::InputText("###BlockID :", &actionGetBlockIDWritten);
		ImGui::SameLine();
		ImGui::Text("Blocks Quantity: ");
		ImGui::SameLine(); 
		ImGui::InputInt("###Blocks Quantity:", &blockQuantityWritten);
		nSpacing(5);
		if (ImGui::Button("Send Message") && neigh[0].first != "Empty") {

			((SPVNode*)currentNode)->getBlockHeader(actionGetBlockIDWritten, std::to_string(blockQuantityWritten), neigh[comboBoxNodesIndex].first);
		}
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void Gui::showNodesTable(NodeFactory& nodes) {

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);

	vector<BaseNode*> node = nodes.getNodes();
	const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	char buffer[32];
	string aux;

	const ImGuiTableFlags flags =
		ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
		| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_NoBordersInBody
		| ImGuiTableFlags_ScrollY;

	ImGui::SetCursorPos(ImVec2(312, 92));
	ImGui::BeginChild("NodeList", ImVec2(480, 300), true);
	if (ImGui::BeginTable("table_sorting", 6, flags))
	{
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, TYPE_TABLE_IDENTIFIER);
		ImGui::TableSetupColumn("IP", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, IP_TABLE_IDENTIFIER);
		ImGui::TableSetupColumn("Port", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, PORT_TABLE_IDENTIFIER);
		ImGui::TableSetupColumn("Active", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, CONNECTIONS_TABLE_IDENTIFIER);
		ImGui::TableSetupColumn("Connections", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, CONNECTIONS_TABLE_IDENTIFIER);
		ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, CONNECTIONS_TABLE_IDENTIFIER);
		ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
		ImGui::TableHeadersRow();


		ImGuiListClipper clipper;
		clipper.Begin(node.size());
		while (clipper.Step())
			for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
			{
				sprintf_s(buffer, "###radio%d", i);
				BaseNode* current = node[i];
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("%s", current->getNodeType().c_str());
				ImGui::TableNextColumn();
				ImGui::Text("%s", current->getIP().c_str());
				ImGui::TableNextColumn();
				ImGui::Text("%s", current->getPort().c_str());
				ImGui::TableNextColumn();
				ImGui::RadioButton(buffer, &currentNodeActive, i);
				ImGui::TableNextColumn();

				if (current->hasNeighbours()) { //Si tiene vecinos

					sprintf_s(buffer, "Show###%d", i);
					if (ImGui::SmallButton(buffer)) {
						ImGui::OpenPopup(buffer);
					}

					vector <pair<string, string>> neigh = current->getNeighbours();

					sprintf_s(buffer, "Connections###%d", i);
					ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
					if (ImGui::BeginPopupModal(buffer, NULL, ImGuiWindowFlags_AlwaysAutoResize))
					{
						for (int i = 0; i < neigh.size(); i++) {
							
							sprintf_s(buffer, "X###%d", i);
							if (ImGui::Button(buffer)) {
								current->deleteNeighbour(i);
							}
							ImGui::SameLine();
							ImGui::Text("IP/Port: %s", neigh[i].first.c_str());
							ImGui::SameLine();
							ImGui::Text("Type: %s", neigh[i].second.c_str());
						}

						if (ImGui::Button("Close", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
						ImGui::SetItemDefaultFocus();
						ImGui::EndPopup();
					}
				}
				else {
					ImGui::Text("%s", "Empty");
				}
				ImGui::TableNextColumn();

				switch(current->getState()) {
					case IDLE:
						ImGui::Text("Idle");
						break;
					case SENDING:
						ImGui::Text("Sending");
						break;
					case RECEIVING:
						aux = "Last message received from " + current->getLastReceivedClient();
						ImGui::Text("Receiving");
						ImGui::SameLine();
						HelpMarker(aux.c_str());
						break;
				}

				blockPage = 0;
			}
		ImGui::EndTable(); 
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
}

static void PushStyleCompact() //TODO: modificar static
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

	nSpacing(4);
	if (blocksQuant > 0) {
		ImGui::Text("Page %d of %d", (int)ceil(blockPage / pageSize + 1), (int)ceil((double)node.getBlockQuant() / (double)pageSize));
	}
	nSpacing(4);
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

		if (node.getNodeType() != "SPV") {
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
		}
		
		nSpacing(1);

		ImGui::NextColumn();
	}

	ImGui::Columns(1);

	//Verificamos que se pueda avanzar o retroceder la pagina que muestra los bloques
	if (blocksQuant > 0) {
		ImGui::SetCursorPos(ImVec2(SCREENWIDTH - 200, 83));
		if (ImGui::Button("Prev", ImVec2(80, 40))) {
			if (pageSize <= blockPage) {
				blockPage -= pageSize;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Next", ImVec2(80, 40))) {
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

void Gui::HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}
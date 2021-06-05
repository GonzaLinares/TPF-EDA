#pragma once
#include "BaseNode.h"

class SPVNode :
    public BaseNode
{
public:

    // Inherited via BaseNode
    virtual bool createBlockchainFromFile(std::string&) override;
    virtual void deleteBlockchain() override;
    virtual Block* getBlock(std::string id) override;
    virtual bool getBlocksID(std::vector<std::string>& buffer, int numOfBlocks = 0, int offset = 0) override;
    virtual int getBlockQuant(void) override;

    //POST HTTP
    bool transactionPost(std::string blockId, std::string host);    //TODO: Preguntar sobre la incoherencia
    bool filterPost();

    //GET HTTP
    bool getBlockHeader(std::string blockId, std::string blockCount, std::string host);

    //FUNCIONES RESPUESTA DE POSTS Y GETS
    bool merkleBlockPostReceived(); 


private:

    static std::vector<std::string> actionsVector;    //Vector con los nombres de las acciones posibles
};

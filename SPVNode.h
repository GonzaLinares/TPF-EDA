#pragma once
#include "BaseNode.h"

/******************************************************************************
* TODO
******************************************************************************/

std::vector<std::string> SPVNode::actionsVector = { "ACA VAN LOS NOMBRES DE LAS ACCIONES" };

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

private:


    static std::vector<std::string> actionsVector;    //Vector con los nombres de las acciones posibles
};

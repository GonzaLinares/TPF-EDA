#pragma once

#include "FullNode.h"

class MinerNode :
    public FullNode
{
public:

    virtual std::string getNodeType() override;

    virtual bool poll() override;

private:
    
    std::vector<Tx> tx2add;
    Block block2mine;

};


#pragma once
#include "BaseNode.h"

class SPVNode :
    public BaseNode
{
public:

    SPVNode(boost::asio::io_context& ioContext, std::string port, std::string path2blockchain);
    SPVNode(boost::asio::io_context& ioContext, std::string port);

    // Inherited via BaseNode
    virtual bool createBlockchainFromFile(std::string&) override;
    virtual std::vector<std::string> getActionList() override;

    //POST HTTP
    bool transactionPost(std::string publicKey, int amount, std::string host);
    bool filterPost(std::string host);

    //GET HTTP
    bool getBlockHeader(std::string blockId, std::string blockCount, std::string host);

    //FUNCIONES RESPUESTA DE POSTS Y GETS
    std::string merkleBlockPostReceived(bool error, int result);

private:

    std::string receivedMsgCB(std::string client, std::string msg);

    static std::vector<std::string> actionsVector;    //Vector con los nombres de las acciones posibles
};

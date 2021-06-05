#pragma once
#include "Connection.h"

#define PORTNUM 80    //Numero de puerto para escuchar

class Server
{
public:

	Server(boost::asio::io_context& _ioContext);

private:

	/*Funcion startListening: Comienza a aceptar conexiones del exterior, se llama automaticamente al instanciar la clase*/
	void startListening();

	/*Callback de conexion, cuando se completa una conexion se llama a esta funcion*/
	void connectionHandler(Connection::pointer newConnection, const boost::system::error_code& error);

	boost::asio::io_context& ioContext;
	tcp::acceptor acceptor;
};


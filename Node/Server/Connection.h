#pragma once

#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "HTTP.h"

using boost::asio::ip::tcp;

class Connection : public boost::enable_shared_from_this<Connection>, public HTTP	//Clase conexion que hereda de HTTP y una clase de boost quie permite el uso de shared pointers
{
public:
	typedef boost::shared_ptr<Connection> pointer;				//Typedef para facilitar declaraciones

	static pointer createConnection(boost::asio::io_context& ioContext);	//Funcion creadora de conexiones

	tcp::socket& getSocket();									//Getter del socket de conexion

	void startHTTP(Connection::pointer thisCon);				//Iniciador de comunicacion

	~Connection();	

private:

	Connection(boost::asio::io_context& ioContext);			
								
	/*Callback para las comunicaciones*/
	void readDataHandler(int recievedBytes, Connection::pointer thisCon, const boost::system::error_code& error);
	void sendDataHandler(int sentBytes, Connection::pointer thisCon, const boost::system::error_code& error);
	
	tcp::socket conSocket;

};


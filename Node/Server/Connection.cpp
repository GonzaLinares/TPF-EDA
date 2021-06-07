#include "Connection.h"


Connection::~Connection()			//Destructor solo para uso de debug
{
#ifdef _DEBUG
	std::cerr << "Connection closed." << std::endl;
#endif // _DEBUG
}

Connection::Connection(boost::asio::io_context& ioContext, boost::function<std::string(std::string, std::string)> postReplyCB)	//Preparo el socket de la conexion con el constructor
	: conSocket(ioContext)
{
	generateReplyData = postReplyCB;
}


Connection::pointer Connection::createConnection(boost::asio::io_context& ioContext, boost::function<std::string(std::string, std::string)> postReplyCB)	//Creador de conexiones inteligentes
{
	return Connection::pointer(new Connection(ioContext, postReplyCB));
}


tcp::socket& Connection::getSocket()		//getter para el socket
{
	return conSocket;
}

//Funcion para escucha por mensaje desde el cliente
void Connection::startHTTP(Connection::pointer thisCon)
{
	//Escucho hasta que el cliente mande el terminador CRLFCRLF
	boost::asio::async_read_until(this->conSocket, boost::asio::dynamic_buffer(this->receivedMsg), "\r\n\r\n",
		boost::bind(&Connection::readDataHandler, this,
			boost::asio::placeholders::bytes_transferred, thisCon, boost::asio::placeholders::error ));
}

//Cuendo recibo un mensaje se llama a este callback para procesarlo
void Connection::readDataHandler( int recievedBytes, Connection::pointer thisCon, const boost::system::error_code& error)
{
	if (!error)
	{

		elaborateMessage(conSocket.remote_endpoint().address().to_string() + ":" + std::to_string(conSocket.remote_endpoint().port()));	//Parseo la entrada de datos y creo una respuesta en toSendMesage

		//Se envia el mensaje de respuesta
		boost::asio::async_write(this->conSocket, boost::asio::dynamic_buffer(this->toSendMsg),
			boost::bind(&Connection::sendDataHandler, this,
				boost::asio::placeholders::bytes_transferred, thisCon, boost::asio::placeholders::error));

#ifdef DEBUGHTTP
		std::cout << "Received request:" << std::endl << this->receivedMsg << std::endl << std::endl;
		std::cout << "Sent response:" << std::endl << this->toSendMsg << std::endl;
#endif // DEBUGHTTP
		this->receivedMsg = "";

	}
	else
	{
#ifdef _DEBUG
		std::cerr << error.message() << std::endl;
#endif // DEBUG
	}
	
}

//Callback para cuendo finaliza el envio de la respuesta
void Connection::sendDataHandler(int sentBytes, Connection::pointer thisCon, const boost::system::error_code& error)
{
	if (error)	
	{
#ifdef _DEBUG
		std::cerr << "Error ocurred while writting: " << error.message() << std::endl;
#endif // DEBUG
	}

	this->conSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_send);	//Como ya se respondio se cierra el socket
}

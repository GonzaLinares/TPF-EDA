#include "Connection.h"


Connection::~Connection()			//Destructor solo para uso de debug
{
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
		std::cout << this->conSocket.local_endpoint().address().to_string() << ":" << std::to_string(this->conSocket.local_endpoint().port()) << " Server Received request:" << std::endl << "*************************" << std::endl << this->receivedMsg << std::endl << "*************************" << std::endl;
		std::cout << this->conSocket.local_endpoint().address().to_string() << ":" << std::to_string(this->conSocket.local_endpoint().port()) << " Server Sent response:" << std::endl << "*************************" << std::endl << this->toSendMsg << std::endl << "*************************" << std::endl;
#endif // DEBUGHTTP
		this->receivedMsg = "";

	}
	else
	{
#ifdef DEBUGSERVER
		std::cerr << error.message() << std::endl;
#endif // DEBUG
	}
	
}

//Callback para cuendo finaliza el envio de la respuesta
void Connection::sendDataHandler(int sentBytes, Connection::pointer thisCon, const boost::system::error_code& error)
{
	if (error)	
	{
#ifdef DEBUGSERVER
		std::cerr << "Error ocurred in " << this->conSocket.local_endpoint().address().to_string() << ":" << std::to_string(this->conSocket.local_endpoint().port()) << " while writting: " << error.message() << std::endl;
#endif // DEBUG
	}
	else
	{
#ifdef DEBUGSERVER
		std::cerr << this->conSocket.local_endpoint().address().to_string() << ":" << std::to_string(this->conSocket.local_endpoint().port()) << " Server connection terminated" << std::endl;
#endif // _DEBUG
	}


	this->conSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_send);	//Como ya se respondio se cierra el socket
}

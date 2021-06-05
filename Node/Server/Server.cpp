#include "Server.h"


Server::Server(boost::asio::io_context& _ioContext, boost::function<std::string(std::string, std::string)> msgReceivedCb_, int portNum)				//Inicializo el servidor y lo pongo a escuchar
	:ioContext(_ioContext),
	acceptor(_ioContext, tcp::endpoint(tcp::v4(), portNum)),
	msgReceivedCb(msgReceivedCb_)
{
	std::cout << msgReceivedCb_("hola", std::to_string(portNum)) << std::endl;
	startListening();
}


void Server::startListening()
{

	Connection::pointer newConnection = Connection::createConnection(ioContext, msgReceivedCb);		//Genero una nuevo conexion con un smart pointer para que se borre al terminar la misma

	acceptor.async_accept(newConnection->getSocket(),
		boost::bind(&Server::connectionHandler, this, newConnection, boost::asio::placeholders::error));	//comienzo a recibir en esa nueva conexion

}

//Cuando hay una nueva conexion se lanza esta funcion
void Server::connectionHandler(Connection::pointer newConnection, const boost::system::error_code& error)
{
	
	if (!error)
	{
		newConnection->startHTTP(newConnection);			//Cuando la conexion se concreta correctamente comienzo a comunicarme de forma asincronica
	}
#ifdef DEBUG
		std::cerr << "Connection status: " << error.message() << std::endl;
#endif // DEBUG

	this->startListening();									//Vuelvo a escuchar por otras comunicaciones
	
}
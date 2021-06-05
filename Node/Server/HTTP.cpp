#include "HTTP.h"

#include <ctime>
#include <fstream>
#include <boost/filesystem.hpp>
#include <sstream>
#include "mime_types.h"

using namespace std;

void HTTP:: elaborateMessage() {

    int error = 0;  //Detecta cuando es momento de enviar el mensaje de error

    ifstream file;

    string auxString;               //String auxiliar con el que realizare los recortes
    string parameters;              //String para los parametros(este server no los utiliza)
    int auxPositionINICIAL = 0;
    int auxPositionFINAL;
    int auxPositionFINALHOLDER;

    auxPositionFINAL = receivedMsg.find(' ', 0 );   //Busco el primer espacio desde el inicio del string

    auxString = receivedMsg.substr(auxPositionINICIAL, auxPositionFINAL);

    if (!error) {   //LECTURA DE COMANDOS
        
        if (!auxString.compare(string("GET"))) {    //Si el comando ingresado es GET...

            command = auxString;
            write_GET_message();
        }
        else if (!auxString.compare(string("VIVA-PERON"))) {    //Si el comando ingresado es VIVA-PERON...


        }
        else {
            error = 1;
            write_error_message();
        }
    }

    auxPositionINICIAL = auxPositionFINAL + 1;      //Apunto al primer caracter del path

    auxPositionFINAL = receivedMsg.find(' ', auxPositionFINAL + 1);     //Optengo el final del path

    auxPositionFINALHOLDER = auxPositionFINAL;     //la guardo para futuro uso             

    auxString = receivedMsg.substr(auxPositionINICIAL, auxPositionFINAL-auxPositionINICIAL);    //Extraigo el path

    auxPositionINICIAL = auxString.find_first_of("&");          //Si hay parametros los extraigo
    if (auxPositionINICIAL != -1)
    {

        parameters = auxString.substr(auxPositionINICIAL, auxPositionFINAL - auxPositionINICIAL);
        auxString.erase(auxPositionINICIAL, auxPositionFINAL - auxPositionINICIAL);                 //borro los parametros del path

    }

    this->path = auxString;                                     //guardo el path
    auxString.erase(0, 1);                                      //Borro la primera /
    boost::filesystem::path path(auxString);

    if (path.empty() || path.string().back() == '/') {          //Si el path esta vacio o no tiene filename, agrego el index.html por default
        auxString += string("index.html");
        path.append(string("index.html"));
    }

    file.open(auxString, ios::in | ios::binary);                //Abro el path y chequeo por errores

    if (file.failbit == 1 || !file.is_open()) {

        cout << "Error al intentar abrir el path" << endl;
        write_error_message();
        error = 1;
    }

    if (!error) {   //LECTURA DEL PATH
        
        auxPositionINICIAL = toSendMsg.find(string("text/html"), 0);
        toSendMsg.replace(auxPositionINICIAL, string("text/html").length(), http::server::mime_types::extension_to_type(boost::filesystem::extension(path)));
    
        stringstream strStream;
        strStream << file.rdbuf(); //read the file

        auxString = strStream.str(); //str holds the content of the file

        auxPositionINICIAL = toSendMsg.find(string("filenameContent"), 0);
        auxPositionFINAL = toSendMsg.find(string("\r\n\r\n"), 0) - auxPositionINICIAL;
        toSendMsg.replace(auxPositionINICIAL, auxPositionFINAL, auxString);    //Escribo todo el contenido

        auxPositionINICIAL = toSendMsg.find("filenameLength", 0);
        auxString = to_string(auxString.size());
        toSendMsg.erase(auxPositionINICIAL, 13);
        toSendMsg.replace(auxPositionINICIAL, 1, auxString);   //Escribo el length

        auxPositionINICIAL = toSendMsg.find("/path/filename", 0);
        toSendMsg.erase(auxPositionINICIAL, 13);
        toSendMsg.replace(auxPositionINICIAL, 1, this->path);   //Escribo el path


        file.close();
        
    }

    auxPositionINICIAL = auxPositionFINALHOLDER + 1;

    auxPositionFINAL = receivedMsg.find(string("\r\n"), 0);    

    auxString = receivedMsg.substr(auxPositionINICIAL, auxPositionFINAL - auxPositionINICIAL);

    auxPositionINICIAL = auxPositionFINAL + 2; //Me muevo al siguiente renglon

    auxPositionFINAL = receivedMsg.find(' ', auxPositionFINAL );

    auxString = receivedMsg.substr(auxPositionINICIAL, auxPositionFINAL - auxPositionINICIAL );

    if (!error) {

        if (!auxString.compare(string("host:")) || !auxString.compare(string("Host:"))) {    //Si se ingreso host: o Host:

            auxPositionINICIAL = auxPositionFINAL + 1;

            auxPositionFINAL = receivedMsg.find(string("\r\n"), auxPositionFINAL + 1);

            auxString = receivedMsg.substr(auxPositionINICIAL, auxPositionFINAL - auxPositionINICIAL);    //quito el ' ' del final tambien

            //en AUXString esta escrito el host:
            host = auxString;
        }
    }

    auxPositionINICIAL = auxPositionFINAL + 2; //Me muevo al siguiente renglon

    auxPositionFINAL = receivedMsg.find(string("\r\n\r\n"), auxPositionFINAL);   //Busco el \r\n\r\n

    if (auxPositionFINAL == -1) {   //no encontro el \r\n\r\n

        error = 1;
        write_error_message();
    }

    auxString = receivedMsg.substr(auxPositionINICIAL, auxPositionFINAL - auxPositionINICIAL); //ACA ESTA EL RESTO DEL STRING, si sirve de algo
}


void HTTP::write_GET_message() {

    char buffer[100];
    int i = 0;

    time_t rawtime = time(0);
    struct tm timeinfo;

    string message = string("HTTP/1.1 200 OK \r\nDate: ");
    message.erase(message.length(), message.length()); //Borro el '\0' del final del string 

    localtime_s(&timeinfo, &rawtime);

    strftime(buffer, 100, "%a, %e %b %G %T GMT", &timeinfo);

    while (buffer[i] != '\0') {

        message.push_back(buffer[i]);
        i++;
    }

    message += string("\r\nLocation: 127.0.0.1/path/filename \r\nCache-Control: max-age = 30 \r\nExpires : ");
    message.erase(message.length(), message.length()); //Borro el '\0\ del final del string 

    rawtime += 30;
    localtime_s(&timeinfo , &rawtime);

    strftime(buffer, 100, "%a, %e %b %G %T GMT", &timeinfo);

    i = 0;

    while (buffer[i] != '\0') {

        message.push_back(buffer[i]);
        i++;
    }

    message += string("\r\nContent-Length: filenameLength \r\nContent-Type: text/html\r\n\nfilenameContent\r\n\r\n");
    message.erase(message.length(), message.length()); //Borro el '\0\ del final del string     ; charset = iso - 8859 - 1

    toSendMsg = message;
}

void HTTP::write_error_message() {

    char buffer[100];
    int i = 0;

    string message = string("HTTP/1.1 404 Not Found\r\nDate : ");
    message.erase(message.length(), message.length()); //Borro el '\0\ del final del string  

    time_t time_written = time(0);
    ctime_s(buffer, 100, &time_written);

    while (buffer[i] != '\n') {

        message.push_back(buffer[i]);
        i++;
    }

    message += string("\r\nCache-Control: public, max-age=30 \r\nExpires: ");
    message.erase(message.length(), message.length()); //Borro el '\0\ del final del string  

    time_written += 30;
    ctime_s(buffer, 100, &time_written);

    i = 0;

    while (buffer[i] != '\n') {

        message.push_back(buffer[i]);
        i++;
    }

    message += string("\r\nContent-Length: 0 \r\nContent-Type: text/html\r\n\r\n");

    toSendMsg = message;
}
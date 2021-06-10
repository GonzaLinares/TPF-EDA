#include "Client.h"
#include <nlohmann/json.hpp>
//#include "procesamiento.h"

using json = nlohmann::json;

static size_t recieveCallback(char* ptr, size_t size, size_t nmemb, void* userdata);


Client::Client(boost::function<std::string(std::string, std::string)> msgReceivedCb_, long port_)
	: err(CURLM_OK),
	status(WAITING),
	msgReceivedCb(msgReceivedCb_),
	curl(NULL),
	list(NULL),
	port(port_)
{
	this->multiCurl = curl_multi_init();
}

Client::~Client()
{
	while (status)
	{
		curl_multi_perform(multiCurl, &status);
	}
	curl_multi_cleanup(multiCurl);
}

bool Client::POST(std::string host, std::string path, std::string& msg)
{
	if ((host + path).size() != 0 && msg.size() != 0 && status == WAITING)
	{
		std::string aux;
		this->toSendMsg = msg;
		receivedData.clear();
		status = DOWNLOADING;
		lastHost = host;
		this->curl = curl_easy_init();


		
		list = curl_slist_append(list, "Content-Type: text/json");

		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		aux = host.substr(0, host.rfind(':')) + "/" + path;
		curl_easy_setopt(curl, CURLOPT_URL, aux.c_str());
		aux = host.substr(host.rfind(':') + 1);
		curl_easy_setopt(curl, CURLOPT_PORT, std::stoi(aux));
		curl_easy_setopt(curl, CURLOPT_LOCALPORT, port);
		curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, recieveCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &receivedData);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, toSendMsg.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, toSendMsg.size());
		curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, TIMEOUTLIMIT);


#ifdef DEBUGCLIENTCURL
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif // _DEBUG

		curl_multi_add_handle(multiCurl, curl);
		curl = NULL;
		
		return true;
	}
	else
	{
		return false;
	}
}

bool Client::GET(std::string host, std::string path)
{
	if ((host + path).size() != 0 && status == WAITING)
	{
		std::string aux;
		receivedData.clear();
		status = DOWNLOADING;
		lastHost = host;
		this->curl = curl_easy_init();
		aux = host.substr(0, host.rfind(':')) + "/" + path;
		curl_easy_setopt(curl, CURLOPT_URL, aux.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPGET);
		curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT);
		aux = host.substr(host.rfind(':') + 1);
		curl_easy_setopt(curl, CURLOPT_PORT, std::stoi(aux));
		curl_easy_setopt(curl, CURLOPT_LOCALPORT, port);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, recieveCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &receivedData);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, TIMEOUTLIMIT);

#ifdef DEBUGCLIENTCURL
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif // _DEBUG

		curl_multi_add_handle(multiCurl, curl);

		return true;
	}
	else
	{
		return false;
	}
}

int Client::getStatus()
{
	return this->status;
}

bool Client::poll()
{
	err = curl_multi_perform(multiCurl, &status);
	int msginQueue = 0;
	CURLMsg* msgs = curl_multi_info_read(multiCurl, &msginQueue);
	if (msgs != NULL && msgs->msg == CURLMSG_DONE)
	{
		msgReceivedCb(lastHost, receivedData);
		curl_easy_cleanup(msgs->easy_handle);
		curl_slist_free_all(list);
		list = NULL;
	}
	else if (msgs != NULL)
	{
		curl_easy_cleanup(msgs->easy_handle);
		curl_slist_free_all(list);
		list = NULL;
	}
	return status;
}

static size_t recieveCallback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
	size_t realsize = size * nmemb;
	std::string* usrdata = (std::string*)userdata;
	usrdata->append(ptr);
	return realsize;
}

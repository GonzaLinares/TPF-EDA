#include "Client.h"
#include <nlohmann/json.hpp>
//#include "procesamiento.h"

using json = nlohmann::json;

static size_t recieveCallback(char* ptr, size_t size, size_t nmemb, void* userdata);


Client::Client(boost::function<std::string(std::string, std::string)> msgReceivedCb_, long port_)
	: err(CURLM_OK),
	status(IDLE),
	msgReceivedCb(msgReceivedCb_),
	curl(NULL),
	port(port_)
{
	this->multiCurl = curl_multi_init();
}

Client::~Client()
{
	curl_multi_cleanup(multiCurl);
	curl_easy_cleanup(curl);
}

bool Client::POST(std::string host, std::string path, std::string& msg)
{
	if ((host + path).size() != 0 && msg.size() != 0 && status == IDLE)
	{
		std::string aux;
		status = DOWNLOADING;
		lastHost = host;
		this->curl = curl_easy_init();

		curl_slist* list = NULL;
		curl_slist_append(list, "Content-Type: text/json");

		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		aux = host.substr(0, host.rfind(':') - 1) + path;
		curl_easy_setopt(curl, CURLOPT_URL, aux.c_str());
		aux = host.substr(host.rfind(':') + 1);
		curl_easy_setopt(curl, CURLOPT_PORT, std::stoi(aux));
		curl_easy_setopt(curl, CURLOPT_LOCALPORT, port);
		curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, recieveCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &receivedData);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, msg.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, msg.size());

#ifdef _DEBUG
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif // _DEBUG


		curl_slist_free_all(list);

		curl_multi_add_handle(multiCurl, curl);

		return true;
	}
	else
	{
		return false;
	}
}

bool Client::GET(std::string host, std::string path)
{
	if ((host + path).size() != 0 && status == IDLE)
	{
		std::string aux;
		status = DOWNLOADING;
		lastHost = host;
		this->curl = curl_easy_init();
		aux = host.substr(0, host.rfind(':')) + "/" + path;
		curl_easy_setopt(curl, CURLOPT_URL, aux.c_str());
		aux = host.substr(host.rfind(':') + 1);
		curl_easy_setopt(curl, CURLOPT_PORT, std::stoi(aux));
		curl_easy_setopt(curl, CURLOPT_LOCALPORT, port);
		curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, recieveCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &receivedData);

#ifdef _DEBUG
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
	}
	else if (msgs != NULL)
	{
		curl_easy_cleanup(msgs->easy_handle);
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

/*
bool Client::download(std::list<std::string>& buffer, const char* usrname, unsigned int tweetCount)
{
	if (this->status == IDLE)
	{

		this->curl = curl_easy_init();
		this->multiCurl = curl_multi_init();
		this->receivedData.clear();

		if (this->curl && this->multiCurl)
		{

			curl_multi_add_handle(this->multiCurl, this->curl);

			//curl_easy_setopt(curl, CURLOPT_URL, );
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);


			curl_easy_setopt(curl, CURLOPT_READFUNCTION, recieveCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &this->receivedData);

		}
		else
		{
			throw std::exception("ERR: Unable to initialize cURL");
		}
	}
	else
	{
		this->err = curl_multi_perform(this->multiCurl, &this->status);
		if (this->status == IDLE)
		{
			if (this->err != CURLM_OK)
			{
				curl_easy_cleanup(this->curl);
				curl_multi_cleanup(this->multiCurl);
				throw std::exception(curl_multi_strerror(this->err));
			}
			else
			{
				curl_easy_cleanup(this->curl);
				curl_multi_cleanup(this->multiCurl);
				json receivedJSON = json::parse(this->receivedData);
				if (1procesamiento(receivedJSON, buffer))
				{

				}
				//TODO: Agregar parseo a lista
				return 0;
			}
		}
	}
	return 1;

}
*/



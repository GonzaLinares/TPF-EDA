#include "Client.h"
#include <nlohmann/json.hpp>
//#include "procesamiento.h"

using json = nlohmann::json;

static size_t recieveCallback(char* ptr, size_t size, size_t nmemb, void* userdata);


Client::Client(boost::function<std::string(std::string, std::string)> msgReceivedCb_)
	: err(CURLM_OK),
	status(IDLE)
{
	this->multiCurl = curl_multi_init();
}

Client::~Client()
{
	curl_multi_cleanup(multiCurl);
	curl_easy_cleanup(curl);
}

bool Client::POST(std::string url, std::string& msg)
{
	if (url.size() != 0 && msg.size() != 0 && status == IDLE)
	{
		status = DOWNLOADING;

		this->curl = curl_easy_init();

		curl_slist* list = NULL;
		curl_slist_append(list, "Content-Type: text/json");

		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, recieveCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &receivedData);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, msg.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, msg.size());

		curl_slist_free_all(list);

		curl_multi_add_handle(&multiCurl, &curl);

		curl_easy_cleanup(&curl);

		return true;
	}
	else
	{
		return false;
	}
}

bool Client::GET(std::string url)
{
	if (url.size() != 0 && status == IDLE)
	{
		status = DOWNLOADING;

		this->curl = curl_easy_init();

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, recieveCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &receivedData);

		curl_multi_add_handle(&multiCurl, &curl);

		curl_easy_cleanup(&curl);

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
	return status;
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


static size_t recieveCallback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
	size_t realsize = size * nmemb;
	std::string* usrdata = (std::string*)userdata;
	usrdata->append(ptr);
	return realsize;
}
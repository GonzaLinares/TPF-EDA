#include "Client.h"
#include <nlohmann/json.hpp>
//#include "procesamiento.h"

using json = nlohmann::json;

static size_t recieveCallback(char* ptr, size_t size, size_t nmemb, void* userdata);


Client::Client()
{
	
	this->curl = curl_easy_init();
	
}

int Client::getStatus()
{
	return this->status;
}

void Client::cancelDownload()
{
	curl_easy_cleanup(this->curl);
	curl_multi_cleanup(this->multiCurl);
	this->status = IDLE;
}


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
			curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);

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
				if (1/*procesamiento(receivedJSON, buffer)*/)
				{

				}
				//TODO: Agregar parseo a lista
				return 0;
			}
		}
	}
	return 1;

}



static size_t recieveCallback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
	size_t realsize = size * nmemb;
	std::string* usrdata = (std::string*)userdata;
	usrdata->append(ptr);
	return realsize;
}
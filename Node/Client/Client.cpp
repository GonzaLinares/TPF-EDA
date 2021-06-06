#include "Client.h"
#include <nlohmann/json.hpp>
//#include "procesamiento.h"

using json = nlohmann::json;

static size_t recieveCallback(char* ptr, size_t size, size_t nmemb, void* userdata);


Client::Client(const char* API_key, const char* API_SecretKey) :
	APIKey(API_key), 
	APISecretKey(API_SecretKey),
	queryLink("https://api.twitter.com/1.1/statuses/user_timeline.json?screen_name={username}&count={tweetcount}")
{
	
	this->curl = curl_easy_init();
	if (curl || API_key == nullptr || API_SecretKey == nullptr)
	{
		curl_easy_setopt(this->curl, CURLOPT_URL, "https://api.twitter.com/oauth2/token");
		curl_easy_setopt(this->curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(this->curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
		curl_easy_setopt(this->curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

		std::string credentials = std::string(this->APIKey) + ":" + std::string(this->APISecretKey);

		curl_easy_setopt(this->curl, CURLOPT_USERPWD, credentials.c_str());

		struct curl_slist* header = nullptr;
		header = curl_slist_append(header, "Content-Type: application/x-www-form-urlencoded;charset=UTF-8");
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, header);

		std::string data = "grant_type=client_credentials";
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDSIZE, data.size());
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, data.c_str());

		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, recieveCallback);
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &this->receivedData);
		if (curl_easy_perform(this->curl) != CURLE_OK)
		{
			curl_easy_cleanup(this->curl);
			throw std::exception("ERR: Failed request to Twitter");

		}

		curl_easy_cleanup(this->curl);

		json recievedJson = json::parse(this->receivedData);
		this->token = recievedJson["access_token"];
		
		this->status = IDLE;

	}
	else
	{
		throw std::exception("ERR: Unable to initialize cURL");
	}
	
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

		std::string usrnameTag = "{username}";
		std::string tweetcountTag = "{tweetcount}";

		if (tweetCount != 0)
		{
			this->queryLink.replace(queryLink.find(usrnameTag), usrnameTag.length(), usrname);
			this->queryLink.replace(queryLink.find(tweetcountTag), tweetcountTag.length(), std::to_string(tweetCount));
		}
		else
		{
			this->queryLink.replace(queryLink.find(usrnameTag), usrnameTag.length(), usrname);
			this->queryLink.replace(queryLink.find(tweetcountTag), tweetcountTag.length(), std::to_string(DEFAULT_TWEET_COUNT));
		}

		this->curl = curl_easy_init();
		this->multiCurl = curl_multi_init();
		this->receivedData.clear();

		if (this->curl && this->multiCurl)
		{

			curl_multi_add_handle(this->multiCurl, this->curl);

			curl_easy_setopt(curl, CURLOPT_URL, this->queryLink.c_str());
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
			curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);

			struct curl_slist* list = nullptr;
			std::string aux = "Authorization: Bearer ";
			aux = aux + token;
			list = curl_slist_append(list, aux.c_str());
			curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, list);

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, recieveCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &this->receivedData);

			curl_multi_perform(this->multiCurl, &this->status);

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
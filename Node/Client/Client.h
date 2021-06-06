#pragma once
#include <iostream>
#include <curl/curl.h>
#include <list>
#include <boost/function.hpp>

#define DEFAULT_TWEET_COUNT 1

enum { IDLE = 0, DOWNLOADING};

class Client
{
public:
	Client(boost::function<std::string(std::string, std::string)> msgReceivedCb_);
	~Client();

	bool POST(std::string url, std::string& msg);
	bool GET(std::string url);
	int getStatus();
	bool poll();

private:
	int status;
	std::string receivedData;
	boost::function<std::string(std::string, std::string)> msgReceivedCb;
	CURL* curl;
	CURLM* multiCurl;
	CURLMcode err;

	
};


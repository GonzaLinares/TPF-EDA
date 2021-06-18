#pragma once
#include <iostream>
#include <curl/curl.h>
#include <list>
#include <boost/function.hpp>

#define TIMEOUTLIMIT 30L

enum { WAITING = 0, DOWNLOADING};

class Client
{
public:
	Client(boost::function<std::string(std::string, std::string)> msgReceivedCb_, long port_);
	~Client();

	bool POST(std::string host, std::string path, std::string& msg);
	bool GET(std::string host, std::string path);
	int getStatus();
	bool poll();

private:
	int status;
	long port;
	std::string toSendMsg;
	std::string receivedData;
	std::string lastHost;
	boost::function<std::string(std::string, std::string)> msgReceivedCb;
	curl_slist* list;
	CURL* curl;
	CURLM* multiCurl;
	CURLMcode err;

	
};


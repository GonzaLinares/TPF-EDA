#pragma once
#include <iostream>
#include <curl/curl.h>
#include <list>
#include <boost/function.hpp>

#define DEFAULT_TWEET_COUNT 1

enum { IDLE = 0, DOWNLOADING, DISPLAYING };

class Client
{
public:
	Client(boost::function<std::string(std::string, std::string)> msgReceivedCb_);

	bool POST(std::string url, std::string& msg);

	int getStatus();
	bool download(std::list<std::string>& buffer, const char* usrname, unsigned int tweetCount = 0);
	void cancelDownload();

private:
	int status;
	std::string receivedData;
	boost::function<std::string(std::string, std::string)> msgReceivedCb;
	CURL* curl;
	CURLM* multiCurl;
	CURLMcode err;

	
};


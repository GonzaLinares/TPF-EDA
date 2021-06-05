#pragma once
#include <iostream>
#include <string>
#include <boost/function.hpp>

class HTTP {
protected:
	std::string receivedMsg;
	std::string toSendMsg;

	std::string path;
	std::string host;
	std::string command;

	boost::function<std::string(std::string, std::string)> generateReplyData;

	void write_response_message();
	void write_error_message();
	void elaborateMessage(std::string client);
};
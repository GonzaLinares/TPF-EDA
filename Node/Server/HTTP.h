#pragma once
#include <iostream>
#include <string>

class HTTP {
protected:
	std::string receivedMsg;
	std::string toSendMsg;

	std::string path;
	std::string host;
	std::string command;

	void write_GET_message();
	void write_error_message();
	void elaborateMessage();
};
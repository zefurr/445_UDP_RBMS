//#include <winsock2.h>
//#include <stdio.h>
//#include <string>
//#include <time.h>
//#include <iomanip>

//#include <iostream>
//#include <chrono>
//#include <ctime>
//#include <time.h>
//#include <sstream>

#pragma once

#include "pch.h"
#include <vector>

// Messages which carry no data (Registration, Acknowledgements)
struct BaseMessage
{
	std::string m_Type;

	BaseMessage();
	BaseMessage(std::string);
};

// Message sent from server to each client to inform them of other participants
struct SessionStartMsg : public BaseMessage
{
	std::vector<std::string> participants;
	SessionStartMsg(std::string, std::vector<std::string>);
};

// Message sent from client to server to request a meeting booking
struct RequestMsg : public BaseMessage
{
};

struct InviteMsg : public BaseMessage
{

};

struct AcceptMsg : public BaseMessage
{

};

struct RejectMsg : public BaseMessage
{

};
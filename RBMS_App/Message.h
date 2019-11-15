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
#include "shared_winsock.h"
#include <vector>

// definitions for messages received from clients
#define REGISTER	"REGISTER"
#define REQ_MEET	"REQUEST"
#define ACCEPT		"ACCEPT"
#define REJECT		"REJECT"
#define WITHDRAW	"WITHDRAW"
#define ADD			"ADD"

// definitions for messages sent by the server
#define SESH_START	"START"
#define ACK_REG		"ACK_REGISTER"
#define INVITE		"INVITE"
#define CANCEL		"CANCEL"
#define ROOM_CHANGE "ROOM_CHANGE"

// Messages which carry no data (Registration, Acknowledgements)
struct BaseMessage
{
	std::string m_Type;
	sockaddr_in m_Destination;

	BaseMessage();
	BaseMessage(std::string, sockaddr_in = { 0 });
	std::string toString();
	std::vector<char> toCharVector();
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
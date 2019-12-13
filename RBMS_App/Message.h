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
#include "Participant.h"
#include <vector>

// definitions for messages sent by clients
#define REGISTER	"REGISTER"
#define REQ_MEET	"REQUEST"
#define ACCEPT		"ACCEPT"
#define REJECT		"REJECT"
#define WITHDRAW	"WITHDRAW"
#define ADD			"ADD"

// definitions for messages sent by the server
#define SESH_START		"START" //both server and client
#define ACK_REG			"ACK_REGISTER"
#define RESPONSE		"RESP"
#define INVITE			"INVITE"
#define REQ_CANCEL		"REQ_CANCEL"
#define CANCEL			"CANCEL"
#define ROOM_CHANGE		"ROOM_CHANGE"
#define UNAVAILABLE		"UNAVAILABLE"
#define SCHEDULED		"SCHEDULED"
#define NOT_SCHEDULED	"NOT_SCHEDULED"
#define WITHDRAWN		"WITHDRAWN"
#define ADDED			"ADDED"
#define CONFIRM			"CONFIRM"

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
	std::string m_Type;
	sockaddr_in m_Destination;
	std::vector<Participant> m_PL;

	SessionStartMsg(std::string type, std::vector<Participant> pl, sockaddr_in = { 0 });
	std::vector<char> toCharVector();
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
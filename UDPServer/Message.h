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
#include "TimeSlot.h"
#include <vector>


// definitions for messages received from clients
#define MSG_REG "REGISTERbutITisREALLYlongINSTEADdoesTHISchangeANYTHING"

// definitions for messaged send by the server
#define MSG_START "START"
#define ACK_REG "ACK_REGISTER"

// Messages which carry no data (Registration, Acknowledgements)
struct BaseMessage
{
	
	std::string msgType;

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
	int reqNum;
	TimeSlot meetTime;
	int minimum_p;
	std::vector<std::string> participants;
	std::string topic;

	RequestMsg(std::string, int, TimeSlot, int, std::vector<std::string>, std::string);
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
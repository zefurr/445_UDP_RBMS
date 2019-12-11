#pragma once

#include "shared_winsock.h"
#include <vector>
#include <thread>
#include <mutex>
#include <string>
#include <sstream> 
#include <condition_variable>
#include <algorithm>
#include "Sender.h"
#include "Message.h"
#include "Participant.h"
#include "Meeting.h"

//RQ# (incremented each for each request message)
extern int requestCounter;
extern int meetingCounter;

class Logic
{
public:

	static Logic& getInstance();
	std::string SItoString(sockaddr_in si);
	void Startup(int);
	void Shutdown();

	//client only knows participant names
	std::vector<std::string> c_pl;
	std::vector<Participant> s_pl;

	static std::vector<std::string> c_meetings;
	static std::vector<Meeting> s_meetings;
	
	void HandleMessage(std::vector<char>, sockaddr_in);

	// User elements start
		//add a new participant
	void AddParticipant(sockaddr_in);
	// function to display the agenda
	void DisplayAgenda(Participant);
	// function to display the participant list
	void DisplayParticipantList();
	//client functions
	void AddClientName(std::string);
	// User elements start

	//message functions

	//RESPONSE|RQ#|UNAVAILABLE
	std::vector<char> CreateRespMessage(std::string);
	//INVITE|MT#|DATE&TIME|TOPIC|REQUESTER
	std::vector<char> CreateInviteMessage(std::string, std::string, std::string, std::string);

private:

	//room1[793] = true;// room1 booked on 79th day, 3th hour
	std::vector<bool> room1;
	std::vector<bool> room2;

	// Basic elements START
	Logic();
	std::thread* m_LogicThread = nullptr;
	int m_Mode;
	bool m_Alive;
	void MainLogic();

	// Basic elements END

	// Consumer elements START
	std::vector<int> m_IntMsgs;
	std::mutex m_Mutex;
	std::condition_variable m_Cond_NotEmpty;
	// Consumer elements END

	// Producer elements START
	Sender& m_Sender = Sender::getInstance();
	// Producer elements END
};


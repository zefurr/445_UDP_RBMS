#pragma once

#include "shared_winsock.h"
#include <vector>
#include <thread>
#include <mutex>
#include <string>
#include <sstream>
#include <condition_variable>
#include <algorithm>
#include <map>

#include "Sender.h"
#include "Logger.h"
#include "Message.h"
#include "Participant.h"
#include "Meeting.h"

#define MAX_TIMEOUTS 3
#define TIMEOUT_WAIT 1000 //in milliseconds

class Logic
{
public:

	static Logic& getInstance();
	int getMsgMax(std::string);
	std::vector<std::string> CharVectorToStringVector(std::vector<char>, sockaddr_in);
	std::string SItoString(sockaddr_in);
	std::string getNameFromSI(sockaddr_in);
	sockaddr_in getSIFromName(std::string);
	bool ScheduleMeeting(std::string mt_nbr);
	void Startup(int);
	void Shutdown();

	//client only knows participant names
	std::vector<Participant> s_pl;

	std::vector<Meeting> s_meetings;

	void HandleMessage(std::vector<char>, sockaddr_in = { 0 });

	//add a new participant
	std::string AddParticipant(sockaddr_in);

	//add meeting to s_meetings
	void AddToAgenda(Meeting);
	// function to display the agenda
	void DisplayAgenda(int status = -1);
	// function to display the participant list
	void DisplayParticipantList();
	//client functions
	void AddClientName(std::string);
	//changes the status of a attendee 
	void ChangeStatus(int newStatus, std::vector<std::string> message, std::string name);
	//client requests server to add client
	void AddToMeeting(std::string meeting_nbr);
	//server success message to inform requester
	void AddedResponse(std::string meeting_nbr,std::string name);
	//client requests server to remove client
	void WithdrawFromMeeting(std::string meeting_nbr);
	//server success message to inform requester
	void WithdrawnResponse(std::string meeting_nbr,std::string name);

	void RequestMeeting(std::string);

	void JoinSession();

	void StartSession();

	void ChangeMeetingRoom(std::string mt_nbr);

	std::string GetMeetingIdFromBooking(std::string room, std::string timeslot);

	void CancelMeeting(std::string mt_nbr, std::string reason);

	void CancelMyMeeting(std::string);

	void RoomChange(std::string timeslot, std::string room);

	//message functions

	std::vector<char> CreateReqMessage(std::string);

	std::vector<char> CreateStartMessage();

	std::vector<char> CreateRegisterMessage();

	std::vector<char> CreateRespMessage(std::string);
	std::vector<char> CreateInviteMessage(std::string, std::string, std::string, std::string, std::string);
	std::vector<char> CreateAckMessage(std::string name);
	std::vector<char> CreateAcceptMessage(std::string meeting_nbr);
	std::vector<char> CreateRejectMessage(std::string meeting_nbr);
	std::vector<char> CreateConfirmMessage(std::string meeting_nbr, std::string room);
	std::vector<char> CreateScheduledMessage(std::string req_nbr, std::string meeting_nbr, std::string room, std::vector<std::string>);
	std::vector<char> CreateNotScheduledMessage(std::string req_nbr, std::string date_time, std::string min, std::vector<std::string> confirmed_attendees, std::string topic);
	std::vector<char> CreateAddMessage(std::string meeting_nbr);
	std::vector<char> CreateAddedMessage(std::string meeting_nbr, std::string client_addr);
	std::vector<char> CreateWithdrawMessage(std::string meeting_nbr);
	std::vector<char> CreateWithdrawnMessage(std::string meeting_nbr, std::string client_addr);

	std::vector<char> CreateRequesterCancelMessage(std::string meeting_nbr);

	std::vector<char> CreateCancelMessage(std::string meeting_nbr, std::string reason);

	//message functions

	bool inSession();
	int participantCount();

	
	std::string getMyName();

private:

	sockaddr_in server_addr;

	int m_meetingCounter = 0;
	int m_requestCounter = 0;
	std::string m_clientName = "unititialized";

	//room1[793] = true;// room1 booked on 79th day, 3th hour
	std::map<std::string, bool> room1;
	std::map<std::string, bool> room2;
	std::map<std::string, bool> m_Timeslot;

	// Basic elements START
	Logic();
	std::thread* m_LogicThread = nullptr;
	int m_Mode;
	bool m_Alive;
	void MainLogic();
	// Basic elements END
	
	bool m_sessionActive = false;

	// Consumer elements START
	std::vector<int> m_IntMsgs;
	std::mutex m_sessionMutex;
	std::condition_variable m_Cond_NotEmpty;
	// Consumer elements END

	// Producer elements START
	Sender& m_Sender = Sender::getInstance();
	Logger& m_Logger = Logger::getInstance();
	// Producer elements END

	std::mutex m_InviteMutex;
	std::mutex m_partiMutex;
	std::mutex m_MeetingMutex;
	std::mutex m_TimeslotMutex;

	int RoomIsAvailable(std::string);

	void SendInvites(std::string, int mode = 0);

	void addToAgenda(Meeting);

	void QueueInvites(std::string meeting_nbr);
	std::vector<std::string> inviteList;

	std::vector<std::thread> vecOfThreads;
	std::vector<std::thread*> vecOfThreadPtrs;

};
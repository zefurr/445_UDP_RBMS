#pragma once
#include "Participant.h"
//#include "Logic.h"
#include <vector>
#include <iostream>

struct Attendee {
	std::string name;
	int status = 0;
	// 0 = No reply
	// 1 = Accept
	// 2 = Reject
	// 3 = Withdraw
};

struct Meeting
{
public:
	int _special_status = 0; // For client only, to know my status for the meeting
	std::string meeting_nbr = "-1";
	std::string req_nbr;
	std::string room = "X"; //room1 or room2
	std::string min_participants;
	std::string confirmed_participants = "0";
	std::string date_time;	// #day #hour
							// 1-99 day
							// 1-9 hour
							// 172 = day 17, hour 2
	std::string topic;
	std::vector<Attendee> attendees;
	std::string requester;

	Meeting();
	Meeting(std::string, std::string, std::string, std::string, std::string);

	void makeFromRequest(std::vector<std::string>, int);
	void makeFromInvite(std::vector<std::string>);

	void setAttendeeStatus(int,std::string);
	void setRoom(int = 0);
	int checkMinAccepts();
	void incrementConfirmed(); // increase confirmed when accept message received
	void decrementConfirmed(); // decrease confirmed when withdraw message received
	//getters
	std::string getMeetingNbr();
	std::string getRequestNbr();
	std::string getRoom();
	std::string getDateTime();
	std::string getMinAttendees();
	std::string getTopic();
	std::string getRequester();
	std::vector<std::string> getAttendees(int);

	void PrintInfo(int);

private:
};
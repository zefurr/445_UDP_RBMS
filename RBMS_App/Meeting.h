#pragma once
#include "Participant.h"
//#include "Logic.h"
#include <vector>
#include <iostream>

struct Attendee {
	std::string name;
	int status = 0;
	// 0 = No reply
	// 1 = Reject
	// 2 = Accept
	// 3 = Withdraw
};

class Meeting
{
private:

	std::string meeting_nbr = "-1";
	std::string req_nbr;
	std::string room = "X";
	std::string min_participants;
	std::string date_time;	// #day #hour
							// 1-99 day
							// 1-9 hour
							// 172 = day 17, hour 2
	std::string topic;
	std::vector<Attendee> attendees;
	std::string requester;



public:
	Meeting();
	Meeting(std::string, std::string, std::string, std::string, std::string);

	void makeFromRequest(std::string);

	//getters
	std::string getMeetingNbr();
	std::string getRequestNbr();
	std::string getRoom();
	std::string getDateTime();
	std::string getTopic();
	std::string getRequester();
	std::vector<std::string> getAttendees(int);

	void PrintInfo();
};
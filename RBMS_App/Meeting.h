#pragma once
#include "Participant.h"
#include <vector>

struct Attendee {
	Participant p;
	int status = 0;
	// 0 = No reply
	// 1 = Reject
	// 2 = Accept
	// 3 = Withdraw
};

class Meeting
{
private:

	//MT#
	std::string meeting_nbr;
	//which room
	std::string room;
	
	// #day #hour
	// 1-99 day
	// 1-9 hour
	// 172 = day 17, hour 2
	std::string date_time;
	//topic
	std::string topic;
	//requester
	std::string requester;

	

	//participants
	std::vector<Attendee> participantsMeeting;
	//int min; // minimum attendees
	
public:
	Meeting();
	Meeting(std::string, std::string, std::string, std::string, std::string);
	~Meeting();
};


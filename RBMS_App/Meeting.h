#pragma once
#include "Participant.h"
//#include "Logic.h"
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

	//id
	int meetingId;
	//time
	int timeslot;
	std::string topic;
	Participant requester;

	//participants
	std::vector<Attendee> participantsMeeting;
	int min; // minimum attendees
	
public:
	Meeting();

	Meeting(std::string, std::string, std::string, std::string, std::string);

	//getters
	std::string getMeetingNbr();
	std::string getRoom();
	std::string getDateTime();
	std::string getTopic();
	std::string getRequester();

	~Meeting();
};


#pragma once

#include "Participant.h"

struct Attendee {
	Participant p;
	int status = 0;
	// 0 = No reply
	// 1 = Reject
	// 2 = Accept
	// 3 = Withdraw
};

struct Request
{
public:
	Request(int, int, int, std::vector<Attendee>, std::string);

	std::vector<char> CreateMessage();

private:
	int req_num;
	int date_time;
	int min;
	std::vector<Attendee> attendees;
	std::string topic;
};


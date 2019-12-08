#pragma once
#include "Participant.h"
#include <vector>
class Meeting
{
private:

	//id
	int meetingId;
	//time
	int timeslot;
	//participants
	std::vector<Participant*> participantsMeeting;


public:
	Meeting();
	~Meeting();
};


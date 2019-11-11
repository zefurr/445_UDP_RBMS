#include "pch.h"
#include "Message.h"

BaseMessage::BaseMessage()
{
	msgType = "Garbage";
}

BaseMessage::BaseMessage(std::string type)
{
	msgType = type;
}

SessionStartMsg::SessionStartMsg(std::string type, std::vector<std::string> participantList)
{
	
	msgType = type;

	participants = participantList;
}

RequestMsg::RequestMsg(std::string type, int requestNumber, TimeSlot meetingTime, int minimumParticipants, std::vector<std::string> participantList, std::string meetingTopic)
{
	msgType = type;

	reqNum = requestNumber;
	meetTime = meetingTime;
	minimum_p = minimumParticipants;
	participants = participantList;
	topic = meetingTopic;
}

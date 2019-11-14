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

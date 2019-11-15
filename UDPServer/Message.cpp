#include "pch.h"
#include "Message.h"

BaseMessage::BaseMessage()
{
	m_Type = "Uninitialized Message!";
}

BaseMessage::BaseMessage(std::string type)
{
	m_Type = type;
}

SessionStartMsg::SessionStartMsg(std::string type, std::vector<std::string> participantList)
{
	m_Type = type;

	participants = participantList;
}

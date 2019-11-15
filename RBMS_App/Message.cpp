#include "pch.h"
#include "Message.h"

BaseMessage::BaseMessage()
{
	m_Type = "Uninitialized Message!";
}

BaseMessage::BaseMessage(std::string type, sockaddr_in destination)
{
	m_Type = type;
	m_Destination = destination;
}

std::string BaseMessage::toString()
{
	using namespace std;

	// Build a string from all the elements of the message
	string str = m_Type;
	str.push_back('|'); // All valid messages must contain at least one | symbol, trailing the type
	
	return str;
}

std::vector<char> BaseMessage::toCharVector()
{
	using namespace std;

	// Build a string from all the elements of the message
	string str = m_Type;
	str.push_back('|'); // All valid messages must contain at least one | symbol, trailing the type
	vector<char> char_vector(str.begin(), str.end());

	return char_vector;
}

SessionStartMsg::SessionStartMsg(std::string type, std::vector<std::string> participantList)
{
	m_Type = type;

	participants = participantList;
}

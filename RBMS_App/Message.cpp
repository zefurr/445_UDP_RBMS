#include "pch.h"
#include "Message.h"
#include <iostream>

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

std::vector<char> BaseMessage::toCharVector() {
	// FORMAT: REGISTER|
	using namespace std;

	// Build a string from all the elements of the message
	string str = m_Type;
	str.push_back('|'); // All valid messages must contain at least one | symbol, trailing the type
	const vector<char> char_vector(str.begin(), str.end());

	return char_vector;
}

SessionStartMsg::SessionStartMsg(std::string type, std::vector<Participant> pl, sockaddr_in destination)
{
	m_Type = type;
	m_PL = pl;
	m_Destination = destination;
}

std::vector<char> SessionStartMsg::toCharVector()
{
	//FORMAT: START|C0,C1,C2|

	using namespace std;

	// Build a string from all the elements of the message
	string str = m_Type;
	str.push_back('|'); // All valid messages must contain at least one | symbol, trailing the type

	for (int i = 0; i < m_PL.size(); i++) {
		if (i != 0) {
			str.append(",");
		}
		str.append(m_PL[i].getClientName());
	}
	str.push_back('|');
	std::cout << "STRRR: " << str << endl;
	const vector<char> char_vector(str.begin(), str.end());
	

	return char_vector;
}

//RequestMsg::RequestMsg(std::string type, int requestNbr, )
//{
//
//}
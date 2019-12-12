#pragma once
#include "shared_winsock.h"
#include <vector>
#include <string>

class Participant
{
private:
	
	//participants only identified ip
	std::string client_addr;
	//name
	std::string client_name;
	//organizer = 1, participant = 0
	bool organizer;
	//amount of participants
	static int clientCounter;

	sockaddr_in client_si;

public:
	Participant();
	Participant(std::string, sockaddr_in);

	//list of availabilities accessed by MS  
	std::vector<std::string> availabilites;

	//getters + setters
	std::string getClientName();
	std::string getClientAddr();
	sockaddr_in getClientSI();
	void setClientAddr(std::string);
	void setClientName(std::string);

};


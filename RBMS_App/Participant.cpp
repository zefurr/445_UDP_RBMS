#include "pch.h"
#include "Participant.h"
#include <string>

//define static int
int Participant::clientCounter;

Participant::Participant() {
}

Participant::Participant(std::string client_addr, sockaddr_in client_si) {
	this->client_addr = client_addr;
	this->client_si = client_si;
	this->client_name = "C" + std::to_string(this->clientCounter);
	//this->availabilites = initialAvailabilities;
	this->organizer = false;

	this->clientCounter++;
}

std::string Participant::getClientName() {
	return this->client_name;
}

std::string Participant::getClientAddr() {
	return this->client_addr;
}

sockaddr_in Participant::getClientSI() {
	return this->client_si;
}

void Participant::setClientAddr(std::string client_addr) {
	this->client_addr = client_addr;
}

void Participant::setClientName(std::string name) {
	this->client_name = name;
}
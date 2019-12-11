#include "pch.h"
#include "Meeting.h"


Meeting::Meeting()
{
}

Meeting::Meeting(std::string meeting_nbr, std::string room, std::string date_time, std::string topic, std::string requester)
{
	this->meeting_nbr = meeting_nbr;
	this->room = room;
	this->date_time = date_time;
	this->topic = topic;
	this->requester = requester;

	//increment MT# 
	//meetingCounter++;
}

std::string Meeting::getMeetingNbr() {
	return this->meeting_nbr;
}


std::string Meeting::getRoom() {
	return this->room;
}

std::string Meeting::getDateTime() {
	return this->date_time;
}

std::string Meeting::getTopic() {
	return this->topic;
}

std::string Meeting::getRequester() {
	return this->requester;
}
Meeting::~Meeting()
{
}

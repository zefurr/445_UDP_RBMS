#include "pch.h"
#include "Meeting.h"

using namespace std;

Meeting::Meeting()
{
}

Meeting::Meeting(string meeting_nbr, string room, string date_time, string topic, string requester)
{
	this->meeting_nbr = meeting_nbr;
	this->room = room;
	this->date_time = date_time;
	this->topic = topic;
	this->requester = requester;

	//increment MT# 
	//meetingCounter++;
}

string Meeting::getMeetingNbr() {
	return this->meeting_nbr;
}

string Meeting::getRequestNbr() {
	return this->req_nbr;
}


string Meeting::getRoom() {
	return this->room;
}

string Meeting::getDateTime() {
	return this->date_time;
}

string Meeting::getTopic() {
	return this->topic;
}

string Meeting::getRequester() {
	return this->requester;
}

vector<string> Meeting::getAttendees(int _status) {
	vector<string> matches;
	for (Attendee a : attendees) {
		if (a.status == _status) {
			matches.push_back(a.name);
		}
	}
	return matches;
}

void Meeting::makeFromRequest(string request_msg) {
	int fieldCounter = 0;
	int plCounter = 0;
	vector<string> req_fields; //field[0] = RQ#
	vector<string> req_pl;

	for (int i = 0; i < request_msg.length(); i++) {
		for (int j = i; j < request_msg.length(); j++) {
			//, only in participant list
			if (request_msg[j] == ',') {
				req_pl[plCounter] = request_msg.substr(i, j);
				plCounter++;
				i = j;
			}
			if (request_msg[j] == '|') {
				req_fields[fieldCounter] = request_msg.substr(i, j);
				fieldCounter++;
				i = j; // Place i at the comma (it will get auto incremented past the comma)
			}
		}
	}
	req_nbr = req_fields[1];
	date_time = req_fields[2];
	min_participants = req_fields[3];
	for (int i = 0; i < plCounter; i++) {
		attendees[i] = { req_pl[i], 0 };
	}
	topic = req_fields[4];
	requester = req_fields[5];
}

void Meeting::PrintInfo() {
	cout << "MT#: " << meeting_nbr << endl;
	cout << "RQ#: " << req_nbr << endl;
	cout << "ROOM#: " << room << endl;
	cout << "Min: " << min_participants << endl;
	cout << "Timeslot: " << date_time << endl;
	cout << "Topic: " << topic << endl;
	cout << "Requester: " << requester << endl;
	cout << "Attendees: " << requester << endl;
	for (Attendee a : attendees) {
		cout << "\tName:" << a.name << " Status: " << a.status << endl;
	}
}
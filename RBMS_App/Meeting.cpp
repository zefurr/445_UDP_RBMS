#include "pch.h"
#include "Meeting.h"

using namespace std;
//TEST
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

void Meeting::setAttendeeStatus(int newStatus, string attendeeName){

	for (Attendee& a : attendees) {
		if (a.name == attendeeName) {
			a.status = newStatus;
		}
	}
}

vector<string> Meeting::getAttendees(int _status) {
	vector<string> matches;
	for (Attendee a : attendees) {
		if (a.status == _status) {
			matches.push_back(a.name);
		}
		if (_status = -1) { // Return ALL attendees
			matches.push_back(a.name);
		}
	}
	return matches;
}

void Meeting::makeFromRequest(string request_msg, int meeting_number) {
	//FORMAT: REQUEST|RQ#|DATE&TIME|MIN_PARTICIPANTS|PARTICIPANTS|TOPIC|REQUESTER
	//		  01234567     
	vector<string> req_fields; //field[1] = RQ#
	vector<string> req_pl;

	for (int i = 0; i < request_msg.length(); i++) {
		for (int j = i; j < request_msg.length(); j++) {
			//, only in participant list
			if (request_msg[j] == ',') {
				cout << req_fields.size() << " - " << request_msg.substr(i, j - i) << endl;
				req_pl.push_back(request_msg.substr(i, j - i));
				i = j + 1;
			}
			if (request_msg[j] == '|') {
				cout << req_fields.size() << " - " << request_msg.substr(i, j - i) << endl;
				req_fields.push_back(request_msg.substr(i, j - i));
				i = j + 1;
			}
		}
	}
	meeting_nbr = to_string(meeting_number);
	req_nbr = req_fields[1];
	date_time = req_fields[2];
	min_participants = req_fields[3];
	for (string name : req_pl) {
		attendees.push_back({ name, 0 });
	}
	attendees.push_back({ req_fields[4], 0 }); // catch the last (or only) attendee
	topic = req_fields[5];
	requester = req_fields[6];
}

void Meeting::makeFromInvite(string invite_msg) {
	//FORMAT: INVITE|MT#|DATE&TIME|TOPIC|REQUESTER
	vector<string> inv_fields; //field[0] = RQ#
	vector<string> req_pl;

	for (int i = 0; i < invite_msg.length(); i++) {
		for (int j = i; j < invite_msg.length(); j++) {
			if (invite_msg[j] == '|') {
				inv_fields.push_back(invite_msg.substr(i, j - i - 1));
				i = j; // Place i at the comma (it will get auto incremented past the comma)
			}
		}
	}
	meeting_nbr = inv_fields[1];
	date_time = inv_fields[2];
	topic = inv_fields[3];
	requester = inv_fields[4];
}

void Meeting::PrintInfo() {
	cout << "MT#: " << meeting_nbr << endl;
	cout << "RQ#: " << req_nbr << endl;
	cout << "ROOM#: " << room << endl;
	cout << "Min: " << min_participants << endl;
	cout << "Timeslot: " << date_time << endl;
	cout << "Topic: " << topic << endl;
	cout << "Requester: " << requester << endl;
	cout << "Attendees: " << endl;
	for (Attendee a : attendees) {
		cout << "\tName:" << a.name << " Status: " << a.status << endl;
	}
}
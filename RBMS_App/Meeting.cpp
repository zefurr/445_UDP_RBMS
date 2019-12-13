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
	this->requester = requester; //name e.g. C1

}

void Meeting::incrementConfirmed(){ // increase confirmed when accept message received
	this->confirmed_participants = to_string(stoi(this->confirmed_participants) + 1);
}

void Meeting::decrementConfirmed() { // decrease confirmed when withdraw message received
	this->confirmed_participants = to_string(stoi(this->confirmed_participants) - 1);
}

//returns 1 if confirmed == min, 2 if confirmed > min, 3 if confirmed < min
int Meeting::checkMinAccepts(){ //triggered whenever a new status
	//check the number of attendees with status accept
	if (stoi(this->confirmed_participants) == stoi(this->min_participants)) {
		//minimum reached and should send out all confirms to all participants
		cout << "Minimum number of accepts reached for meeting: " + this->meeting_nbr << endl;
		return 1;
	}  
	else if (stoi(this->confirmed_participants) > stoi(this->min_participants)){
		//meeting already confirmed, send add message for individual late confirm
		cout << "Meeting has already been confirmed, new participant to be added for meeting: " + this->meeting_nbr << endl;
		return 2;
	}
	else {
		//no messages to be sent, meeting not confirmed yet
		cout << "Minimum number of accepts insufficient for meeting: " + this->meeting_nbr << endl;
		return 3;
	}
}


void Meeting::setAttendeeStatus(int newStatus, string attendeeName){
	for (Attendee& a : attendees) {
		//find attendee using attendeeName param
		if (a.name == attendeeName) {
			//set new status
			int oldStatus = a.status;
			a.status = newStatus; //1 = accept, 2 = reject, 3 = withdraw, 0 = no reply yet

			if (newStatus == 1 && oldStatus != 1) {
				//increments confirmed participants in meeting
				incrementConfirmed();
				cout << "Status of " << a.name << " changed to ACCEPT for MT#: " << meeting_nbr << endl;
			}
			else if (newStatus == 2 && oldStatus != 2) {
				cout << "Status of " << a.name << " changed to REJECT for MT#: " << meeting_nbr << endl;
			}
			else if (newStatus == 3 && oldStatus != 3) {
				//decrements confirmed participants in meeting
				decrementConfirmed();
				cout << "Status of " << a.name << " changed to WITHDRAW for MT#: " << meeting_nbr << endl;
			}
		}
	}
}

void Meeting::setRoom(int room_number) {
	if (room_number == 1) {
		this->room = "room1";
	}
	else if (room_number == 2) {
		this->room = "room2";
	}
	else {
		this->room = "X";
	}
}

vector<string> Meeting::getAttendees(int _status) {
	vector<string> matches;
	for (Attendee& a : attendees) {
		if (a.status == _status) {
			matches.push_back(a.name);
		}
		if (_status == -1) { // Return ALL attendees
			matches.push_back(a.name);
		}
	}
	return matches;
}

void Meeting::makeFromRequest(vector<string> request_msg, int meeting_number) {
	//FORMAT: REQUEST|RQ#|DATE&TIME|MIN_PARTICIPANTS|PARTICIPANTS|TOPIC|REQUESTER
	//		  01234567                              |C0,C1,C2,C3|
	meeting_nbr = to_string(meeting_number);
	req_nbr = request_msg[1];
	date_time = request_msg[2];
	min_participants = request_msg[3];
	// for loop handle below attendees from request_msg[4]
	topic = request_msg[5];
	requester = request_msg[6];

	for (int i = 0; i < request_msg[4].length(); i++) {
		for (int j = i; j < request_msg[4].length(); j++) {
			if (request_msg[4][j] == ',') {
				// Every comma you find marks the end of a participant name
				// Add the participant as an attendee with status 0 (no reply)
				// Increment i to point to the character following the comma
				// j will naturally increment to the same position
				attendees.push_back({ request_msg[4].substr(i, j - i), 0 });
				i = j + 1;
			}

			if (j == request_msg[4].length() - 1) {
				// If you are at the last character of the participant list string
				// This must be the last character of a participant name
				// Add the participant as an attendee with status 0 (no reply)
				// Unlike in the case of a comma, j does not point past the name so add +1
				attendees.push_back({ request_msg[4].substr(i, j - i + 1), 0 });
				i = j;
			}
		}
	}
}

void Meeting::makeFromInvite(vector<string> invite_msg) {
	//FORMAT: INVITE|MT#|DATE&TIME|TOPIC|REQUESTER|
	meeting_nbr = invite_msg[1];
	date_time = invite_msg[2];
	topic = invite_msg[3];
	requester = invite_msg[4];
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

string Meeting::getMinAttendees() {
	return this->min_participants;
}

string Meeting::getTopic() {
	return this->topic;
}

string Meeting::getRequester() {
	return this->requester;
}

#include "pch.h"
#include "Logic.h"

#include <iostream> // for debugging

using namespace std;

//intialize counters at 0
int requestCounter = 0;
int meetingCounter = 0;

vector<bool> room1 = {};
vector<bool> room2 = {};

Logic& Logic::getInstance()
{
	static Logic _instance;
	return _instance;
}

int Logic::getMsgMax(string msg_type) {
	// Based on the type of message we know how many fields need to exist
	// Return that number of fields

	int msg_max_fields = 0;
	if (msg_type == REGISTER) {
		// FORMAT: REGISTER|
		msg_max_fields = 1;
	}
	else if (msg_type == REQ_MEET) {
		//FORMAT: REQUEST|RQ#|DATE&TIME|MIN_PARTICIPANTS|C0,C1,C2|TOPIC|REQUESTER|
		msg_max_fields = 7;
	}
	else if (msg_type == ACCEPT) {
		//FORMAT: ACCEPT|MT#|
		msg_max_fields = 2;
	}
	else if (msg_type == REJECT) {
		//FORMAT: REJECT|MT#|
		msg_max_fields = 2;
	}
	else if (msg_type == WITHDRAW) {
		//FORMAT: WITHDRAW|MT#|
		msg_max_fields = 2;
	}
	else if (msg_type == WITHDRAWN) {
		//FORMAT: WITHDRAW|MT#|REQUESTER_ADDR|
		msg_max_fields = 3;
	}
	else if (msg_type == ADD) {
		//FORMAT: ADD|MT#|
		msg_max_fields = 2;
	}
	else if (msg_type == ADDED) {
		//FORMAT: ADDED|MT#|REQUESTER_ADDR|
		msg_max_fields = 3;
	}
	else if (msg_type == SESH_START) {
		//FORMAT: START|C0,C1,C2|
		msg_max_fields = 2;
	}
	else if (msg_type == ACK_REG) {
		// FORMAT: ACK_REGISTER|C0|
		msg_max_fields = 2;
	}
	else if (msg_type == RESPONSE) {
		// FORMAT: RESPONSE|RQ#|UNAVAILABLE|
		msg_max_fields = 3;
	}
	else if (msg_type == INVITE) {
		//FORMAT: INVITE|MT#|DATE&TIME|TOPIC|REQUESTER|
		msg_max_fields = 5;
	}
	else if (msg_type == CONFIRM) {
		//FORMAT: CONFIRM|MT#|ROOM#|
		msg_max_fields = 3;
	}
	else if (msg_type == SCHEDULED) {
		//FORMAT: SCHEDULED|RQ#|MT#|ROOM#|C0,C1,C2|
		msg_max_fields = 5;
	}
	else if (msg_type == CANCEL) {
		//FORMAT: CANCEL|MT#|REASON|
		msg_max_fields = 3;
	}
	else if (msg_type == NOT_SCHEDULED) {
		//FORMAT: NOT_SCHEDULED|RQ#|DATE&TIME|MIN_PARTICIPANTS|CONFIRMED_PARTICIPANTS|TOPIC|
		msg_max_fields = 6;
	}
	else if (msg_type == ROOM_CHANGE) {
		//FORMAT: ROOM_CHANGE|MT#|NEW_ROOM#|
		msg_max_fields = 3;
	}

	return msg_max_fields;
}

vector<string> Logic::CharVectorToStringVector(vector<char> char_vector, sockaddr_in src_addr) {
	vector<string> conversion;

	string raw_content(char_vector.begin(), char_vector.end());
	int first_delim = raw_content.find_first_of('|');

	if (first_delim == string::npos) {
		// Invalid message format.
		cout << " Invalid message format." << endl;
	}
	else {
		int last_delim = raw_content.find_last_of('|');
		string msg_content = raw_content.substr(0, last_delim + 1);
		m_Logger.Log(msg_content.append(" from " + SItoString(src_addr)));

		// Look at the first field of the char vector
		// Each field ends with a | character

		// Based on the first character, the number of fields for each type of message is known

		// Keep going until you find all of the fields

		int max_fields = msg_content.length(); // Dummy initial value

		for (int i = 0; conversion.size() < max_fields && i < msg_content.length(); i++) {
			for (int j = i; conversion.size() < max_fields && j < msg_content.length(); j++) {
				if (msg_content[j] == '|') {
					//cout << conversion.size() << " - " << msg_content.substr(i, j - i) << endl; // debug
					conversion.push_back(msg_content.substr(i, j - i));
					i = j + 1;
					if (conversion.size() == 1) {
						// Only read until the | trailing the last field
						// (this avoids the risk of finding a | in the garbage)
						max_fields = getMsgMax(conversion[0]); 
					}
				}
			}
		}
	}
	return conversion;
}

string Logic::SItoString(sockaddr_in si) {
	string client_addr;
	stringstream ss;
	ss << inet_ntoa(si.sin_addr) << ":" << ntohs(si.sin_port);
	client_addr = ss.str();
	return client_addr;
}

string Logic::getNameFromSI(sockaddr_in si){
	for (Participant& p : s_pl){
		if (p.getClientAddr() == SItoString(si)){
			return p.getClientName();
		}
	}
	return "Participant not found !";
}

sockaddr_in Logic::getSIFromName(string name) {
	for (Participant& p : s_pl) {
		if (p.getClientName() == name) {
			return p.getClientSI();
		}
	}
	cout << "Participant not found !";
	return sockaddr_in { 0 };
}


// If the meeting is not scheduled, tries to schedule the meeting
// Returns true if the meeting is schduled (has a room booked)
// Returns false if there is no room booked and none were available
bool Logic::ScheduleMeeting(string mt_nbr) {
	bool scheduled = false;
	for (Meeting& m : s_meetings) {
		if (m.getMeetingNbr() == mt_nbr) {
			if (m.getRoom() != "X") {
				scheduled = true;
			}
			else {
				m.setRoom(RoomIsAvailable(m.getDateTime()));
				//"book" the room inside of the map by setting the timeslot to true
				if (m.getRoom() == "room1") {
					room1.insert_or_assign(m.getDateTime(), true);
					scheduled = true;
				}
				else if (m.getRoom() == "room2") {
					room2.insert_or_assign(m.getDateTime(), true);
					scheduled = true;
				}
			}
			break; // We found the meeting no need to continue
		}
	}
	return scheduled;
}

//create participant and add to server side participant list
string Logic::AddParticipant(sockaddr_in si)
{
	lock_guard<mutex> participantlock(m_partiMutex);
	Participant p(SItoString(si), si);
	s_pl.push_back(p);
	return p.getClientName();

	/*string client_addr;
	stringstream ss;
	ss << inet_ntoa(si.sin_addr) << ":" << ntohs(si.sin_port);
	client_addr = ss.str();*/

	/*if (find(participantlist.begin(), participantlist.end(), p) != participantlist.end())
	{
	cout << "Client is already registered !" << endl;
	}
	else
	{

		s_pl.push_back(p);
	cout << "Client with name " + p.getClientName() + " and with address " + p.getClientAddr() + " successfully registered." << endl;
	cout << "Total clients: " << s_pl.size() << endl;*/

	//}
}

void Logic::AddToAgenda(Meeting m) {
	lock_guard<mutex> meetinglock(m_MeetingMutex);
	s_meetings.push_back(m);
}

void Logic::DisplayAgenda(int status) {
	lock_guard<mutex> meetinglock(m_MeetingMutex);
	for (Meeting& m : s_meetings) {
		m.PrintInfo();
	}
}

void Logic::DisplayParticipantList() {
	lock_guard<mutex> participantlock(m_partiMutex);
	for (Participant& p : s_pl) {
		cout << p.getClientName();
		if (p.getClientAddr() != "") {
			cout << " " << p.getClientAddr();
		}
		cout << endl;
	}
}

void Logic::AddClientName(string name) {
	Participant p;
	p.setClientName(name);
	s_pl.push_back(p);
}

//functionalities: 
//1. set new attendee status
//2. checks if confirmed equal to min or if its over min
//3. call create confirm and send to all attendees 
//4. create scheduled message and send out to requester
void Logic::ChangeStatus(int newStatus, vector<string> message, string name){
	string meeting = message[1]; //MT#

	// get the source address

	// find the matching participant name
	for (Participant& p : s_pl) {
		if (p.getClientName() == name) {
			// for all participants
			// if they match the participant name
			// set their status

			for (int i = 0; i < s_meetings.size(); i++) {
				//find meeting
				if (s_meetings[i].getMeetingNbr() == meeting) {
					vector<string> attendees = s_meetings[i].getAttendees(-1);

					for (string& a : attendees) {
						//find attendee
						if (a == p.getClientName()) {
							//change status of attendee according to newStatus param
							s_meetings[i].setAttendeeStatus(newStatus, a);

							//check if min reached
							if (s_meetings[i].checkMinAccepts() == 1) { // If the meeting has just been confirmed
								vector<string> acceptedAttendees = s_meetings[i].getAttendees(1); //returns names of all accepted attendees
								if (ScheduleMeeting(s_meetings[i].getMeetingNbr())) {
									//create confirm message
									//send out confirm to all attendees who have accepted
									
									vector<char> confirm = CreateConfirmMessage(s_meetings[i].getMeetingNbr(), s_meetings[i].getRoom());

									for (string& a : acceptedAttendees) {
										for (Participant& p : s_pl) {
											if (a == p.getClientName()) {
												m_Sender.SendUDPMessage(confirm, p.getClientSI());
											}
										}
									}

									//create scheduled message
									//send out to requester
									m_Sender.SendUDPMessage(
										CreateScheduledMessage(
											s_meetings[i].getRequestNbr(),
											s_meetings[i].getMeetingNbr(),
											s_meetings[i].getRoom(),
											s_meetings[i].getAttendees(1)
										),
										getSIFromName(s_meetings[i].getRequester())
									);
								}
								else { // Meeting could not be scheduled because rooms were all full
									vector<char> cancel = CreateCancelMessage(meeting, "Below_Minimum");

									for (string& a : acceptedAttendees) {
										for (Participant& p : s_pl) {
											if (a == p.getClientName()) {
												m_Sender.SendUDPMessage(cancel, p.getClientSI());
											}
										}
									}

									m_Sender.SendUDPMessage(
										CreateNotScheduledMessage(
											s_meetings[i].getRequestNbr(),
											s_meetings[i].getDateTime(),
											s_meetings[i].getMinAttendees(),
											s_meetings[i].getAttendees(1),
											s_meetings[i].getTopic()
										),
										getSIFromName(s_meetings[i].getRequester())
									);
								}
							}
							else if (s_meetings[i].checkMinAccepts() == 2) { //late confirm to be added individually
								//create a single confirm message
								//send out to the attendee that was added


								m_Sender.SendUDPMessage(
									CreateConfirmMessage(
										s_meetings[i].getMeetingNbr(),
										s_meetings[i].getRoom()
									),
									p.getClientSI()
								);
							}
							else if (s_meetings[i].checkMinAccepts() == 3){ //withdraw
								//if this withdrawal causes the confirmed to fall below min
								//server will send a round of invites to all the attendees who have not accepted
								//if still not enough confirmed -> cancel the meeting
							}
						}
					}
					break; // Meeting was found
				}
				// We could not find the meeting specified. Assume it was cancelled
				// Send a cancellation
				m_Sender.SendUDPMessage(CreateCancelMessage(meeting, "Not_Scheduled"), getSIFromName(name));
			} // for (int i = 0; i < s_meetings.size(); i++) {
		}
	}
}

// Only called by client (requester)
void Logic::RequestMeeting(string requester_name) {
	// Build a request message
	vector<char> mycharvector = CreateReqMessage(requester_name);

	// Make a meeting from the request, implicitly booking myself at that time slot
	Meeting m;
	m.makeFromRequest(CharVectorToStringVector(mycharvector, { 0 }), -1);

	// Send the request to the server
	m_Sender.SendUDPMessage(mycharvector, server_addr);
	
	// TBD is this meeting valid? 
	// If I'm free for the time slot
		//book me for that meeting
		//send the request to the server
	// Else
		//put the meeting in a separate vector
		//alert requester they are booked (so they can either change the request or make themselves available)
}

void Logic::AddToMeeting(string meeting_nbr){
	
	//if client was previously invited to meeting
	for (Meeting& m : s_meetings) {
		if(m.getMeetingNbr() == meeting_nbr){
			// book my local calendar for that meeting
			m.setAttendeeStatus(1, getMyName());
			//create add message, send to server
			m_Sender.SendUDPMessage(CreateAddMessage(meeting_nbr), server_addr);
			return;
		} 
	}
	cout << "You have not received an invite for meeting " << meeting_nbr << endl;
}

void Logic::WithdrawFromMeeting(string meeting_nbr) {
	
	//if client was previously invited to meeting
	for (Meeting& m : s_meetings) {
		if (m.getMeetingNbr() == meeting_nbr){
			//clear my local calendar for that meeting
			m.setAttendeeStatus(3, getMyName());
			//create withdraw message, send to server
			m_Sender.SendUDPMessage(CreateWithdrawMessage(meeting_nbr), server_addr);
				
			return;
		}
	}
	cout << "You are not attending meeting #" << meeting_nbr << endl;
}

//void Logic::AddedResponse(string meeting_nbr, string name) {
//	//create added message
//	//inform organizer an attendee was added
//	//here we want to send the client_addr of the requester
//	for (Participant& p : s_pl) { //find requester in s_pl
//		if (p.getClientName() == name) {
//			m_Sender.SendUDPMessage(
//				CreateAddedMessage( //message passed to handle message, then call change status with new status = accept
//					meeting_nbr,
//					p.getClientAddr()
//				),
//				p.getClientSI() //sent to requester
//			);
//		}
//	}
//}

//void Logic::WithdrawnResponse(string meeting_nbr, string name){
//	//create added message
//		//inform organizer an attendee was added
//		//here we want to send the client_addr of the requester
//	for (Participant& p : s_pl) { //find requester in s_pl
//		if (p.getClientName() == m.getRequester()) {
//			m_Sender.SendUDPMessage(
//				CreateWithdrawnMessage( //message passed to handle message, then call change status with new status = accept
//					meeting_nbr,
//					p.getClientAddr()
//				),
//				p.getClientSI() //sent to requester
//			);
//		}
//	}
//}

//FUNCTIONS FOR MESSAGES START

vector<char> Logic::CreateReqMessage(string requester_name) {
	string userTemp = "";
	string str = REQ_MEET;
	str.push_back('|');
	str.append(to_string(requestCounter));
	str.push_back('|');

	//date and time

	cout << "Enter date and time of the meeting: (111-999)" << endl;
	cout << "First 2 digits signify day (1-99) and 3rd is the hour (1-9) " << endl;
	cin >> userTemp;
	while (stoi(userTemp) < 111 && stoi(userTemp) > 999) {
		cout << "Wrong format, try again with a number between 111-999 !" << endl;
		cin >> userTemp;
	}
	str.append(userTemp);
	str.push_back('|');

	//min participants
	cout << "Enter minimum number of participants:" << endl;
	cout << "Max: " << s_pl.size() << endl;
	cin >> userTemp;
	while (stoi(userTemp) > s_pl.size()) {
		cout << "Maximum exceeded, enter another number !" << endl;
		cin >> userTemp;
	}
	str.append(userTemp);
	str.push_back('|');
	
	//create participant list
	cout << "Enter names of participants for the meeting:" << endl;
	cout << "Enter -1 when you are done adding." << endl;
	bool first_time = true;
	while (userTemp != "-1") {
		cin >> userTemp;
		if (userTemp != "-1") {
			for (int i = 0; i < s_pl.size(); i++){
				if (s_pl[i].getClientName() != userTemp) {
					cout << "Client does not exist !" << endl;
				}
				else {
					if (!first_time) {
						str.push_back(',');
					}
					first_time = false;
					str.append(userTemp);
				}
			}
		}
	}
	str.push_back('|');

	//topic
	cout << "Enter topic of the meeting:" << endl;
	cin >> userTemp;
	str.append(userTemp);
	str.push_back('|');

	str.append(requester_name);
	str.push_back('|');

	const vector<char> char_vector(str.begin(), str.end());
	return char_vector;
}

vector<char> Logic::CreateStartMessage() {
	//FORMAT: START|C0,C1,C2|

	string str = SESH_START;
	str.push_back('|'); // All valid messages must contain at least one | symbol, one after each field

	lock_guard<mutex> participantlock(m_partiMutex);
	for (int i = 0; i < s_pl.size(); i++) {
		if (i != 0) {
			str.append(",");
		}
		str.append(s_pl[i].getClientName());
	}
	str.push_back('|');

	const vector<char> char_vector(str.begin(), str.end());

	return char_vector;
}

vector<char> Logic::CreateRegisterMessage() {
	string str = REGISTER;
	str.push_back('|'); // All valid messages must contain at least one | symbol, one after each field

	const vector<char> char_vector(str.begin(), str.end());

	return char_vector;
}

vector<char> Logic::CreateRespMessage(std::string rq_nbr) {
	// FORMAT: RESPONSE|RQ#|UNAVAILABLE|
	string str = RESPONSE;
	str.push_back('|'); // All valid messages must contain at least one | symbol, one after each field
	str.append(rq_nbr);
	str.push_back('|');
	str.append(UNAVAILABLE);
	str.push_back('|');

	const vector<char> char_vector(str.begin(), str.end());

	return char_vector;
}

vector<char> Logic::CreateInviteMessage(std::string mt_nbr, std::string date_time, std::string topic, std::string requester) {
	// FORMAT: INVITE|MT#|DATETIME|TOPIC|REQUESTER|
	string str = INVITE;
	str.push_back('|'); // All valid messages must contain at least one | symbol, one after each field
	str.append(mt_nbr);
	str.push_back('|');
	str.append(date_time);
	str.push_back('|');
	str.append(topic);
	str.push_back('|');
	str.append(requester);
	str.push_back('|');

	const vector<char> char_vector(str.begin(), str.end());

	return char_vector;
}

vector<char> Logic::CreateAckMessage(string name) {
	// FORMAT: ACK_REGISTER|C0|
	string str = ACK_REG;
	str.push_back('|'); // All valid messages must contain at least one | symbol, one after each field
	str.append(name);
	str.push_back('|');

	const vector<char> char_vector(str.begin(), str.end());

	return char_vector;
}

vector<char> Logic::CreateAcceptMessage(string meeting_nbr){
	// FORMART: ACCEPT|MT#|
	string str = ACCEPT;
	str.push_back('|'); // All valid messages must contain at least one | symbol, one after each field
	str.append(meeting_nbr);
	str.push_back('|');

	const vector<char> char_vector(str.begin(), str.end());

	return char_vector;
}

vector<char> Logic::CreateRejectMessage(string meeting_nbr) {
	// FORMAT: REJECT|MT#|
	string str = REJECT;
	str.push_back('|'); // All valid messages must contain at least one | symbol, one after each field
	str.append(meeting_nbr);
	str.push_back('|');

	const vector<char> char_vector(str.begin(), str.end());

	return char_vector;
}

vector<char> Logic::CreateConfirmMessage(string meeting_nbr, string room) {
	//FORMAT: CONFIRM|MT#|ROOM#|
	string str = CONFIRM;
	str.push_back('|'); // All valid messages must contain at least one | symbol, one after each field
	str.append(meeting_nbr);
	str.push_back('|');
	str.append(room);
	str.push_back('|');

	const vector<char> char_vector(str.begin(), str.end());

	return char_vector;
}

vector<char> Logic::CreateScheduledMessage(string req_nbr, string meeting_nbr, string room, vector<string> confirmed_attendees) {
	//FORMAT: SCHEDULED|RQ#|MT#|ROOM#|C0,C1,C2|
	string str = SCHEDULED;
	str.push_back('|'); // All valid messages must contain at least one | symbol, one after each field
	str.append(req_nbr);
	str.push_back('|');
	str.append(meeting_nbr);
	str.push_back('|');
	str.append(room);
	str.push_back('|');

	for (int i = 0; i < confirmed_attendees.size(); i++) {
		if (i != 0) {
			str.append(",");
		}
		str.append(confirmed_attendees[i]);
	}
	str.push_back('|');

	const vector<char> char_vector(str.begin(), str.end());

	return char_vector;
}

vector<char> Logic::CreateNotScheduledMessage(string req_nbr, string date_time, string min, vector<string> confirmed_attendees, string topic) {
	//FORMAT: NOT_SCHEDULED|RQ#|DATETIME|MIN|C0,C1,C2|TOPIC|
	string str = SCHEDULED;
	str.push_back('|'); // All valid messages must contain at least one | symbol, one after each field
	str.append(req_nbr);
	str.push_back('|');
	str.append(date_time);
	str.push_back('|');
	str.append(min);
	str.push_back('|');
	for (int i = 0; i < confirmed_attendees.size(); i++) {
		if (i != 0) {
			str.append(",");
		}
		str.append(confirmed_attendees[i]);
	}
	str.push_back('|');
	str.append(topic);
	str.push_back('|');

	const vector<char> char_vector(str.begin(), str.end());

	return char_vector;
}

vector<char> Logic::CreateAddMessage(string meeting_nbr){
	//FORMAT: ADD|MT#|
	string str = ADD;
	str.push_back('|'); // All valid messages must contain at least one | symbol, one after each field
	str.append(meeting_nbr);
	str.push_back('|');

	const vector<char> char_vector(str.begin(), str.end());
	return char_vector;
}

vector<char> Logic::CreateAddedMessage(string meeting_nbr, string client_addr) {
	//FORMAT: ADDED|MT#|CLIENT_ADDR|
	string str = ADDED;
	str.push_back('|'); // All valid messages must contain at least one | symbol, one after each field
	str.append(meeting_nbr);
	str.push_back('|');
	str.append(client_addr);
	str.push_back('|');

	const vector<char> char_vector(str.begin(), str.end());
	return char_vector;
}

vector<char> Logic::CreateWithdrawMessage(string meeting_nbr) {
	//FORMAT: WITHDRAW|MT#|
	string str = WITHDRAW;
	str.push_back('|'); // All valid messages must contain at least one | symbol, one after each field
	str.append(meeting_nbr);
	str.push_back('|');

	const vector<char> char_vector(str.begin(), str.end());
	return char_vector;
}

vector<char> Logic::CreateWithdrawnMessage(string meeting_nbr, string client_addr) {
	//FORMAT: WITHDRAWN|MT#|CLIENT_ADDR|
	string str = WITHDRAWN;
	str.push_back('|'); // All valid messages must contain at least one | symbol, one after each field
	str.append(meeting_nbr);
	str.push_back('|');
	str.append(client_addr);
	str.push_back('|');

	const vector<char> char_vector(str.begin(), str.end());
	return char_vector;
}

vector<char> Logic::CreateCancelMessage(string meeting_nbr, string reason) {
	// FORMAT: CANCEL|MT#|REASON|
	string str = CANCEL;
	str.push_back('|'); // All valid messages must contain at least one | symbol, one after each field
	str.append(meeting_nbr);
	str.push_back('|');
	str.append(reason);
	str.push_back('|');

	const vector<char> char_vector(str.begin(), str.end());
	return char_vector;
}


//MESSAGES END

int Logic::RoomIsAvailable(string date_time) {
	//room1[date_time] == false;

	//bool first = room1[date_time];
	//bool second = room2[date_time];

	if (room1[date_time] != true) {
		return 1;
	}
	else if (room2[date_time] != true) {
		return 2;
	}

	return 0;
}

// TBD thread this, and add timeout
void Logic::SendInvites(string req_nbr) {
	vector<string> invitees;
	Meeting m;

	{
		lock_guard<mutex> meetinglock(m_MeetingMutex);
		for (Meeting a : s_meetings) {
			if (a.getRequestNbr() == req_nbr) {
				invitees = a.getAttendees(0); // 0 means haven't replied yet
				m = a;
			}
		}
	}

	for (string person : invitees) {
		for (Participant p : s_pl) {
			if (p.getClientName() == person) {
				m_Sender.SendUDPMessage(
					CreateInviteMessage(
						m.getMeetingNbr(),
						m.getDateTime(),
						m.getTopic(),
						m.getRequester()
					),
					p.getClientSI()
				);
			}
		}
	}

	// Match each invitee to an entry in the participant list
	// retrieve their socket address and send an invite
	/*for (int k = 0; k < invitees.size(); k++) {
		for (int q = 0; q < s_pl.size(); q++) {
			if (invitees[k] == s_pl[q].getClientName()) {
				m_Sender.SendUDPMessage(
					CreateInviteMessage(
						m.getMeetingNbr(),
						m.getDateTime(),
						m.getTopic(),
						m.getRequester()
					),
					s_pl[q].getClientSI()
				);
			}
		}
	}*/
}

void Logic::addToAgenda(Meeting m) {
	lock_guard<mutex> meetinglock(m_MeetingMutex);
	s_meetings.push_back(m);
}

void Logic::HandleMessage(std::vector<char> message, sockaddr_in src_addr)
{
	// Send the message to the logger
		// Better as string or char vector? Does it even matter?

	

	string raw_content(message.begin(), message.end());
	int first_delim = raw_content.find_first_of('|');
	
	if (first_delim == string::npos) {
		// Invalid message format.
		cout << " Invalid message format." << endl;
	}
	else {
		vector<string> msg = CharVectorToStringVector(message, src_addr);
	
		if (m_Mode == 1) // Server logic
		{
			if (msg[0] == REGISTER) { // FORMAT: REGISTER|
				// A client wishes to register for this session

				// Add them to the participants list and retrieve their assigned name
				string name = AddParticipant(src_addr);
				// Reply with an acknowledgement of their request informing them of their assigned name
				m_Sender.SendUDPMessage(CreateAckMessage(name), src_addr);
			}
			else if (msg[0] == REQ_MEET) { //FORMAT: REQUEST|RQ#|DATE&TIME|MIN_PARTICIPANTS|PARTICIPANTS|TOPIC|REQUESTER|  
				// A client wishes to create a meeting
				

				// Parse the message into a meeting
				// if room unavailable
					// reply to requester
				// else room is available
					// create a thread that will
						// send invites out
						// wait a certain amount of time
						// check if we have enough responses
							// book the meeting
						// else
							// cancel the meeting

				Meeting m;
				m.makeFromRequest(msg, m_meetingCounter++);

				if (RoomIsAvailable(m.getDateTime()) != 0) {
					// We know a room is available right now (but it might become booked during the invitation process)
					// Regardless, create the meeting object so we can use it to send out invites
					s_meetings.push_back(m);
					// Create a thread to send out invites
					SendInvites(m.getRequestNbr()); // TBD thread this
				}
				else { // Respond to the requester to let them know no rooms are available
					m_Sender.SendUDPMessage(CreateRespMessage(m.getRequestNbr()), src_addr);
				}

			}
			else if (msg[0] == ACCEPT) { //FORMAT: ACCEPT|MT#|
				// A client is accepting a meeting invitation
				//change status to accept for client who sent msg
				ChangeStatus(2, msg, getNameFromSI(src_addr));
			}
			else if (msg[0] == REJECT) { //FORMAT: REJECT|MT#|
				// A client is rejecting a meeting invitation
				//change status to reject for client who sent msg
				ChangeStatus(1, msg, getNameFromSI(src_addr));
			}
			else if (msg[0] == ADD) { //FORMAT: ADD|MT#|
				// A client is joining a previously rejected meeting
				// Change the status of client to accept which will trigger a confirm
				ChangeStatus(1, msg, getNameFromSI(src_addr));
				//send response to inform requester of add
				//find addr of requester

				//find addr of requester
				for (Meeting& m : s_meetings) {
					if (msg[1] == m.getMeetingNbr()) {
						for (Participant& p : s_pl) {
							if (p.getClientName() == m.getRequester()) {
								m_Sender.SendUDPMessage(CreateAddedMessage(msg[1], getNameFromSI(src_addr)), p.getClientSI());
								break;
							}
						}
						//WithdrawnResponse(msg[1], m.getRequester());
						break;
					}
				}


				/*for (Meeting& m : s_meetings){
					if (msg[1] == m.getMeetingNbr()){
						AddedResponse(msg[1], m.getRequester());
					}
				}*/
			}
			else if (msg[0] == WITHDRAW) { //FORMAT: WITHDRAW|MT#|
				// A client is withdrawing attendance from a meeting
				// Change the status of client to withdraw
				// This could potentially cancel the meeting, but don't worry about that here
				ChangeStatus(3, msg, getNameFromSI(src_addr));
				//send response to inform requester of the withdrawn participant

				//find addr of requester
				for (Meeting& m : s_meetings) {
					if (msg[1] == m.getMeetingNbr()) {
						for (Participant& p : s_pl) {
							if (p.getClientName() == m.getRequester()) {
								m_Sender.SendUDPMessage(CreateWithdrawnMessage(msg[1], getNameFromSI(src_addr)), p.getClientSI());
								break;
							}
						}
						//WithdrawnResponse(msg[1], m.getRequester());
						break;
					}
				}
			}
			else { // Unsupported message (either invalid or meant for client logic)

			}
		}
		else { // Client logic (m_Mode == 0 ) 
			if (msg[0] == SESH_START) { //FORMAT: START|C0,C1,C2|
				// Session has begun, server is providing participant list
				// We need to go through the received list and build a client-side copy

				for (int i = 0; i < msg[1].length(); i++) {
					for (int j = i; j < msg[1].length(); j++) {
						if (msg[1][j] == ',') {
							// Every comma you find marks the end of a participant name
							// Add the participant
							// Increment i to point to the character following the comma
							// j will naturally increment to the same position
							AddClientName(msg[1].substr(i, j - i));
							i = j + 1;
						}

						if (j == msg[1].length() - 1) {
							// If you are at the last character of the participant list string
							// This must be the last character of a participant name
							// Add the participant
							// Unlike in the case of a comma, j does not point past the name so add +1
							AddClientName(msg[1].substr(i, j - i + 1));
							i = j;
						}
					}
				}

				lock_guard<mutex> seshlock(m_sessionMutex);
				m_sessionActive = true;


				//string name = "";

				//// Find all the participants
				//// Format is |c0,c1,c2,c3,c4|...more unwanted content...|...|
				//for (int i = 0; i < msg[1].length; i++) {
				//	if (msg[1][i] == '|') {
				//		if (name.empty()) {
				//			name = msg_content.substr(first_delim + 1, last_delim - first_delim - 1);
				//			cout << "ONLY ONE NAME: " << name << endl;
				//			AddClientName(name);
				//		}
				//		break; // We reached the end of the participant list
				//	}
				//	for (int j = i; j < last_delim; j++) {
				//		// until you see ,
				//		if (msg_content[j] == ',') {
				//			cout << "FOUND ," << endl;
				//			// We've reached the next participant
				//			//populate client side participant list
				//			name = msg_content.substr(i, j - i - 1);
				//			AddClientName(name);
				//			i = j; // Place i at the comma (it will get auto incremented past the comma)
				//		}
				//	}
				//}

				
			}
			else if (msg[0] == ACK_REG) { // FORMAT: ACK_REGISTER|C0|
				// Server has received your registration request
				// Wait for confirmation of the session start

				//m_clientName = msg_content.substr(first_delim + 1, last_delim - first_delim - 1); // trim off surrounding pipes ('|')

				m_clientName = msg[1];
				cout << m_clientName << endl;
			}
			//else if (msg[0] == REQ_MEET) { // FORMAT: REQUEST|RQ#|DATE&TIME|MIN_PARTICIPANTS|C0,C1,C2|TOPIC|REQUESTER|
			//	// Format irrelevant. We forward an entire char vector
			//	// We have created a meeting request, forward it to the server
			//	m_Sender.SendUDPMessage(message, server_addr);
			//}
			else if (msg[0] == RESPONSE) { // FORMAT: RESPONSE|RQ#|UNAVAILABLE|
				// No meeting rooms available for the current timeslot
				//room is unavailable
				cout << "No meeting rooms available for the selected timeslot !" << endl;
			}
			else if (msg[0] == INVITE) { // FORMAT: INVITE|MT#|DATE&TIME|TOPIC|REQUESTER|
				// Server is forwarding a meeting invitation
				// build the meeting from the invite
				Meeting m;
				m.makeFromInvite(msg);
				
				//push m into s_meetings
				addToAgenda(m); // We may need to look at this meeting later

				lock_guard<mutex> timelock(m_TimeslotMutex);
				if (m_Timeslot[m.getDateTime()] == false) {
					m_Timeslot[m.getDateTime()] = true; // Book me
					// I am available at that time slot, send accept message
					m_Sender.SendUDPMessage(CreateAcceptMessage(m.getMeetingNbr()), server_addr);
				}
				else {
					// I am unavailable at that time slot, send reject message
					m_Sender.SendUDPMessage(CreateRejectMessage(m.getMeetingNbr()), server_addr);
				}
			}
			else if (msg[0] == CONFIRM) { //FORMAT: CONFIRM|MT#|ROOM#|
				//sent to all accepted attendees, success message for room has been scheduled
				cout << "Meeting #" << msg[1] << " is confirmed in " << msg[2] << endl;
			}
			else if (msg[0] == SCHEDULED) { //FORMAT: SCHEDULED|RQ#|MT#|ROOM#|(CONFIRMED)C0,C1,C2|
				//sent to requester, success message for room has been scheduled
				cout << "Meeting " << msg[2] << " made from request " << msg[1] << "scheduled in " << msg[3] << endl;
			}
			else if (msg[0] == ADDED) { //FORMAT: ADDED|MT#|CLIENT_ADDR|
				//sent to all accepted attendees, success message for room has been scheduled
				cout << "Client with address " << msg[2] << "has been added to meeting " << msg[1] << endl;
			}

			else if (msg[0] == WITHDRAWN) { //FORMAT: WITHDRAWN|MT#|CLIENT_ADDR|
				//sent to all accepted attendees, success message for room has been scheduled
				cout << "Client with address " << msg[2] << "has been removed from meeting " << msg[1] << endl;
			}
			else if (msg[0] == CANCEL) { //FORMAT: CANCEL|MT#|REASON|
				// A meeting involving this client has been cancelled
				// Clear that meeting from the agenda (implicitly making this client available at that meeting time)

			}
			else if (msg[0] == ROOM_CHANGE) { //FORMAT: ROOM_CHANGE|MT#|NEW_ROOM#|
				// A meeting this client is attending has changed room
				// If that meeting exists in our agenda
					// Update the room
			}
			else { // Unsupported message (either invalid or meant for server logic)
			}
		}
	}
}

Logic::Logic()
	: m_Mode(0) // Assume client logic if not specified
{
	memset((char *)&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER);
}

void Logic::MainLogic() {
	//while (m_Alive) {
	//	// aquire a lock on the queue
	//	unique_lock<mutex> lock(m_Mutex);
	//	// wait for notify from SendUDPMessage() function
	//	m_Cond_NotEmpty.wait(lock,
	//		[&a = m_Alive, &mList = m_IntMsgs]
	//	{ return (!a || !mList.empty()); });
	//	// To prevent spurious wake up make sure either 
	//	// we are not alive OR message list isnt empty

	//// copy the messages to a local var
	//	vector<int> copy{ m_IntMsgs };

	//	// clear the message list
	//	m_IntMsgs.clear();

	//	// release lock
	//	lock.unlock();

	//	// send messages via the socket
	//	for (int i : copy)
	//	{
	//		// TBD Read the message, do a switch case based on the type of message
	//		// TBD This varies greatly on whether we are a client or a server
	//		m_Sender.SendUDPMessage(i);
	//	}
	//}
}

void Logic::Startup(int mode)
{
	m_Mode = mode; // 0 = Client, 1 = Server
	m_Alive = true;

	m_Logger.SetMode(mode);
	m_LogicThread = new thread{ &Logic::MainLogic, this }; // Start logic first, it's harmless on its own
	m_Sender.Startup(m_Mode); // Start sender before receiver, otherwise we might get a message we can't reply to
	//m_Receiver.Startup(mode); // Start receiver third, if we get a message we may need logic and sender
}

void Logic::Shutdown()
{
	m_Alive = false;
	m_Cond_NotEmpty.notify_one();

	// Think about the order of shutting these down
	// If we send a message but expect a reply, we should keep the receiver open until sender thread terminates
	// If we receive a message and need to reply, we should keep the sender open until we do
	// Logic will be the one issuing these instructions, so where does it land?
	cout << "Shutdown Receiver...";
	//m_Receiver.Shutdown(); // TBD recvfrom is blocking, can't shutdown // Circular dependency
	cout << "DONE" << endl;
	cout << "Shutdown Sender...";
	m_Sender.Shutdown();
	cout << "DONE" << endl;

	if (m_LogicThread->joinable()) {
		m_LogicThread->join();
	}
	delete m_LogicThread; // consider checking if (m_Thread != nullptr)

	// Close the socket
}

bool Logic::inSession() {
	lock_guard<mutex> seshlock(m_sessionMutex);
	return m_sessionActive;
}

int Logic::participantCount() {
	lock_guard<mutex> participantlock(m_partiMutex);
	return s_pl.size();
}

string Logic::getMyName() {
	return m_clientName;
}

void Logic::JoinSession() {
	m_Sender.SendUDPMessage(CreateRegisterMessage(), server_addr);
}

void Logic::StartSession() {
	vector<char> startmessage = CreateStartMessage(); // This function call lock_guard on m_partiMutex
	lock_guard<mutex> participantlock(m_partiMutex);
	for (Participant& p : s_pl) {
		m_Sender.SendUDPMessage(startmessage, p.getClientSI());
	}
}

void Logic::ChangeMeetingRoom(string mt_nbr) {
	// When this is called m_TimeslotMutex is already held
	// (thus we can assume we can take the timeslot of the other room)

	// When this is called m_MeetingMutex is already held
	for (Meeting& m : s_meetings) {
		if (m.getMeetingNbr() == mt_nbr) {
			if (m.getRoom() == "room1") {
			}
		}
	}
}

string Logic::GetMeetingIdFromBooking(string room, string timeslot) {
	// When this is called m_MeetingMutex is already held
	string mt_nbr = "";

	for (Meeting& m : s_meetings) {
		if (m.getRoom() == room && m.getDateTime() == timeslot) {
			mt_nbr = m.getMeetingNbr();
		}
	}
	if (mt_nbr == "") {
		cout << "No meeting found for given booking timeslot" << endl;
	}
	return mt_nbr;
}

void Logic::CancelMeeting(string mt_nbr, string reason) {
	// When this is called m_MeetingMutex is already held
	for (Meeting& m : s_meetings) {
		if (m.getMeetingNbr() == mt_nbr) {
			vector<string> attendees = m.getAttendees(1); // get confirmed attendees
			vector<char> cancellation_msg = CreateCancelMessage(mt_nbr, reason);

			lock_guard<mutex> participantlock(m_partiMutex);
			for (Participant& p : s_pl) {
				for (string a : attendees) {
					if (a == p.getClientName()) {
						m_Sender.SendUDPMessage(cancellation_msg, p.getClientSI());
					}
				}
			}
		}

	}
}

void Logic::RoomChange(string room, string timeslot) {
	// Room change is authorative. That means it always wins in a contest about a timeslot
	
	bool room1busy;
	bool room2busy;

	lock_guard<mutex> timemutex(m_TimeslotMutex);
	room1busy = (room1[timeslot] == true);
	room2busy = (room2[timeslot] == true);
	

	if (stoi(room) == 1) {
		if (room1busy) {
			lock_guard<mutex> meetinglock(m_MeetingMutex);
			string mt_nbr = GetMeetingIdFromBooking(room, timeslot);
			if (room2busy) { 
				// The room stays booked (by the server)
				// cancel the meeting
				CancelMeeting(mt_nbr, "ROOM_MAINTENANCE");
			}
			else { // Room 2 is available at that time slot
				// move the meeting from room 1 to room 2
				ChangeMeetingRoom(mt_nbr);
			}
		}
	}
	else if (stoi(room) == 2) {
		if (room2busy) {
			lock_guard<mutex> meetinglock(m_MeetingMutex);
			string mt_nbr = GetMeetingIdFromBooking(room, timeslot);
			if (room1busy) { 
				// The room stays booked (by the server)
				// cancel the meeting
				CancelMeeting(mt_nbr, "ROOM_MAINTENANCE");
			}
			else {// Room 1 is available at that time slot
				// move the meeting from room 2 to room 1
				ChangeMeetingRoom(mt_nbr);
			}
		}
	}
}
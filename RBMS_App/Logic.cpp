#include "pch.h"
#include "Logic.h"

#include <iostream> // for debugging

using namespace std;

//intialize counters at 0
int requestCounter = 0;
int meetingCounter = 0;

Logic& Logic::getInstance()
{
	static Logic _instance;
	return _instance;
}

string Logic::SItoString(sockaddr_in si) {
	string client_addr;
	stringstream ss;
	ss << inet_ntoa(si.sin_addr) << ":" << ntohs(si.sin_port);
	client_addr = ss.str();
	return client_addr;
}

//create participant and add to server side participant list
string Logic::AddParticipant(sockaddr_in si)
{
	string client_addr;
	stringstream ss;
	ss << inet_ntoa(si.sin_addr) << ":" << ntohs(si.sin_port);
	client_addr = ss.str();

	Participant p(client_addr, si);


	/*if (find(participantlist.begin(), participantlist.end(), p) != participantlist.end())
	{
	cout << "Client is already registered !" << endl;
	}
	else
	{*/

	s_pl.push_back(p);
	cout << "Client with name " + p.getClientName() + " and with address " + p.getClientAddr() + " successfully registered." << endl;
	cout << "Total clients: " << s_pl.size() << endl;
	//}

	return p.getClientName();
}

void Logic::DisplayAgenda()
{
	lock_guard<mutex> meetinglock(m_MeetingMutex);
	for (Meeting& m : s_meetings) {
		m.PrintInfo();
	}
}

void Logic::DisplayParticipantList() {

}

void Logic::AddClientName(string name) {

}

//message functions
vector<char> Logic::CreateReqMessage(string requester_name) {
	string userTemp = "";
	string str = "REQUEST";
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
	while (stoi(userTemp) != -1) {
		cin >> userTemp;
		for (int i = 0; i < s_pl.size(); i++){
			if (s_pl[i].getClientName() != userTemp) {
				cout << "Client does not exist !" << endl;
			}
			else {
				str.append(userTemp);
				if (i != s_pl.size()-1) {
					str.push_back(',');
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

vector<char> Logic::CreateRespMessage(std::string rq_nbr) {
	// Build a string from all the elements of the message
	string str = "RESP";
	str.push_back('|'); // All valid messages must contain at least one | symbol, trailing the type
	str.append(rq_nbr);
	str.push_back('|');
	str.append("UNAVAILABLE");
	str.push_back('|');

	const vector<char> char_vector(str.begin(), str.end());

	return char_vector;
}

vector<char> Logic::CreateInviteMessage(std::string mt_nbr, std::string date_time, std::string topic, std::string requester)
{
	// Build a string from all the elements of the message
	string str = "INVITE";
	str.push_back('|'); // All valid messages must contain at least one | symbol, trailing the type
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
	string str = ACK_REG;
	str.push_back('|'); // All valid messages must contain at least one | symbol, trailing the type
	str.append(name);
	str.push_back('|');

	const vector<char> char_vector(str.begin(), str.end());

	return char_vector;
}

bool Logic::RoomIsAvailable(string date_time) {
	return room1[stoi(date_time)] || room2[stoi(date_time)];
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

	// Match each invitee to an entry in the participant list
	// retrieve their socket address and send an invite
	for (int k = 0; k < invitees.size(); k++) {
		for (int q = 0; q < s_pl.size(); q++) {
			if (invitees[k] == s_pl[q].getClientAddr()) {
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
	}
}

void Logic::HandleMessage(std::vector<char> message, sockaddr_in src_addr)
{
	// Send the message to the logger
		// Better as string or char vector? Does it even matter?

	string raw_content(message.begin(), message.end());
	int first_delim = raw_content.find_first_of('|');
	int last_delim = raw_content.find_last_of('|');

	string msg_content(message.begin(), message.begin() + last_delim);
	
	if (first_delim == string::npos) {
		// Invalid message format.
	}
	else {
		string msg_type = raw_content.substr(0, first_delim);
	
		if (m_Mode == 1) // Server logic
		{
			if (msg_type == REGISTER) { // A client wishes to register for this session

				// Add them to the participants list in memory
				string name = AddParticipant(src_addr);
				// Reply with an acknowledgement of their request
				m_Sender.SendUDPMessage(CreateAckMessage(name), src_addr);
			}
			else if (msg_type == REQ_MEET) { // A client wishes to create a meeting
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

				m.makeFromRequest(msg_content);

				if (RoomIsAvailable(m.getDateTime())) {
					// Create a thread to send out invites
					SendInvites(m.getRequestNbr()); // TBD thread this
				}
				else { // Respond to the requester to let them know no rooms are available
					m_Sender.SendUDPMessage(CreateRespMessage(m.getRequestNbr()), src_addr);
				}

				//int fieldCounter = 0;
				//int plCounter = 0;
				//vector<string> req_fields; //field[0] = RQ#
				//vector<string> req_pl;

				//for (int i = first_delim + 1; i < last_delim + 1; i++) {
				//	for (int j = i; j < last_delim + 1; j++) {
				//		//, only in participant list
				//		if (msg_content[j] == ',') {
				//			cout << "FOUND ," << endl;
				//			req_pl[plCounter] = msg_content.substr(i, j);
				//			plCounter++;
				//			i = j;
				//		}
				//		if (msg_content[j] == '|') {
				//			cout << "FOUND |" << endl;
				//			req_fields[fieldCounter] = msg_content.substr(i, j);
				//			cout << "FIELD FOUND: " << req_fields[fieldCounter] << endl;
				//			fieldCounter++;
				//			i = j; // Place i at the comma (it will get auto incremented past the comma)
				//		}
				//	}
				//}
				//// Room is unavailable
				//string freeRoom = "Room1";
				//if (!room1[stoi(req_fields[1])]){
				//	freeRoom = "Room2";
				//	if (!room2[stoi(req_fields[1])]) {
				//		freeRoom = "";
				//		for (int k = 0; k < req_pl.size(); k++) {
				//			for (int q = 0; q < s_pl.size(); q++) {
				//				if (req_pl[k] == s_pl[q].getClientAddr()) {
				//					m_Sender.SendUDPMessage(
				//						CreateRespMessage(req_fields[0]),
				//						s_pl[q].getClientSI()
				//					);
				//				}
				//			}
				//		}
				//	}
				//}
				//Room is available
				//else {
				//	// Build meeting object
				//	Meeting m (to_string(m_meetingCounter), freeRoom, req_fields[1], req_fields[4], SItoString(src_addr));
				//	m_meetingCounter++;
				//	// Start sending invitations
				//	for (int k = 0; k < req_pl.size(); k++) {
				//		for (int q = 0; q < s_pl.size(); q++) {
				//			if (req_pl[k] == s_pl[q].getClientAddr()) {
				//				m_Sender.SendUDPMessage(
				//					CreateInviteMessage(
				//						m.getMeetingNbr(),
				//						m.getDateTime(),
				//						m.getTopic(),
				//						m.getRequester()
				//					),
				//					s_pl[q].getClientSI()
				//				);
				//			}
				//		}
				//	}
				//}
			}
			else if (msg_type == ACCEPT) { // A client is accepting a meeting invitation
				// If the meeting exists
					// If the client is invited to that meeting
						// Set the flag for that client to (ACCEPTED)
						// Increment the number of confirmations for that meeting
			}
			else if (msg_type == REJECT) { // A client is rejecting a meeting invitation
				// If the meeting exists
					// If the client is invited to that meeting
						// Set the flag for that client to (REJECTED)
			}
			else if (msg_type == WITHDRAW) { // A client is withdrawing attendance from a meeting
				// If the meeting exists
					// If the client is invited to that meeting
						// Set the flag for that client to (WITHDRAW)
						// Decrement the number of confirmations for that meeting
			}
			else if (msg_type == ADD) { // A client is joining a previously rejected meeting
				// If the meeting exists
					// If the client is invited to that meeting
						// Set the flag for that client to (ACCEPT)
						// Increment the number of confirmations for that meeting
						// If the room for the meeting has previously changed
							// Send a ROOM_CHANGE to the client
				// Else it doesn't exist
					// Send a CANCEL to the client
			}
			else if (msg_type == SESH_START) { // User at server terminal has ended the registration period
				//  Provide all participants with participant list
				//string serialized = SerializeParticipantList(participantlist);

				for (int i = 0; i < s_pl.size(); i++) {
					SessionStartMsg startSession(msg_type, s_pl, s_pl[i].getClientSI());
					/*cout << "TYPE " << startSession.m_Type << endl;
					cout << "NAME " << startSession.m_Participant << endl;*/
					m_Sender.SendUDPMessage(startSession.toCharVector(), startSession.m_Destination);
				}	
			}
			else { // Unsupported message (either invalid or meant for client logic)

			}
		}
		else { // Client logic (m_Mode == 0 ) 
			sockaddr_in server_addr;
			memset((char *)&server_addr, 0, sizeof(server_addr));
			server_addr.sin_family = AF_INET;
			server_addr.sin_port = htons(PORT);
			server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER);

			if (msg_type == SESH_START) { // Session has begun, server is providing participant list


				// *** OVER HERE

				string name = "";

				// Find all the participants
				for (int i = first_delim + 1; i < last_delim + 1; i++) {
					if (msg_content[i] == '|') {
						if (name.empty()) {
							name = msg_content.substr(first_delim + 1, last_delim - 1);
							cout << "ONLY ONE NAME: " << name << endl;
							AddClientName(name);
						}
						break; // We reached the end of the participant list
					}
					for (int j = i; j < last_delim; j++) {
						// until you see ,
						if (msg_content[j] == ',') {
							cout << "FOUND ," << endl;
							// We've reached the next participant
							//populate client side participant list
							name = msg_content.substr(i, j);
							AddClientName(name);
							i = j; // Place i at the comma (it will get auto incremented past the comma)
						}
					}
				}

				// *** UNTIL HERE
				
				//cout << "Participant list: " << msg_content << endl;

				//// Find all the participants
				//// Format is |c0,c1,c2,c3,c4|...more unwanted content...|...|
				//for (int i = first_delim + 1; i < last_delim; i++) {
				//	if (msg_content[i] == '|') {

				//		break; // We reached the end of the participant list
				//	}
				//	for (int j = i; j < last_delim; j++) {
				//		// until you see ,
				//		if (msg_content[j] == ',') {
				//			// We've reached the next participant
				//			//String name = msg_content(i, j);
				//			Participant p;
				//			p.setClientName(msg_content.substr(i, j - 1));
				//			s_pl.push_back(p);
				//			i = j; // Place i at the comma (it will get auto incremented past the comma)
				//		}
				//	}
				//}
				lock_guard<mutex> seshlock(m_sessionMutex);
				m_sessionActive = true;
			}
			else if (msg_type == ACK_REG) { // Server has received your registration request
				// Wait for confirmation of the session start
				m_clientName = msg_content.substr(first_delim + 1, last_delim - 1); // trim off trailing pipes ('|')
			}
			else if (msg_type == REQ_MEET) { // We have created a meeting request, send it to the server

			}
			else if (msg_type == INVITE) { // Server is forwarding a meeting invitation
				// Check agenda

				// If available reply with ACCEPT

				// Else unavailable reply with REJECT
			}
			else if (msg_type == CANCEL) { // A meeting involving this client has been cancelled
				// Clear that meeting from the agenda (implicitly making this client available at that meeting time)
			}
			else if (msg_type == ROOM_CHANGE) { // A meeting this client is attending has changed room
				// If that meeting exists in our agenda
					// Update the room
			}
			else if (msg_type == REGISTER) { // This client wishes to register with the server
				// TBD this is silly: we made a message in RBMS_App
				// TBD - then converted it to vector + sockaddr
				// TBD - then converted the vector to string
				// TBD - now we're making a message from string + sockaddr
				// TBD - should just keep it as a message from start to finish
				BaseMessage reg_client(msg_type, server_addr); // src_addr somewhat misleading when we get client commands
				m_Sender.SendUDPMessage(reg_client.toCharVector(), server_addr);
			}
			else { // Unsupported message (either invalid or meant for server logic)
			}
		}
	}
}

Logic::Logic()
	: m_Mode(0) // Assume client logic if not specified
{
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

// Only called by client (requester)
void Logic::RequestMeeting(string requester_name) {
	vector<char> mycharvector = CreateReqMessage(requester_name);

	HandleMessage(mycharvector);

	// Make a meeting out of the request

	// If I'm free for the time slot
		//book me for that meeting
		//send the request to the server
	// Else
		//put the meeting in a separate vector
		//alert requester they are booked (so they can either change the request or make themselves available)


	// is this meeting valid? 

}

int Logic::participantCount() {
	lock_guard<mutex> partilock(m_partiMutex);
	return 1;
}

string Logic::getMyName() {
	return m_clientName;
}
#include "pch.h"
#include "Logic.h"

#include <iostream> // for debugging 

using namespace std;
//declare extern vector
vector<Participant> participantlist;
vector<Meeting> meetings;
//vector<string> participantlist;
//vector<string> meetings;
vector<int> room1;
vector<int> room2;

//intialize counters at 0
int requestCounter = 0;

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

void Logic::AddParticipant(sockaddr_in si) 
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
		participantlist.push_back(p);
	//	cout << "Client with name " + p.getClientName() + " and with address " + p.getClientAddr() + " successfully registered." << endl;
	//}
}

string Logic::SerializeParticipantList(vector<string> vs)
{
	string serialized = "";

	for (int i = 0; i < vs.size(); i++) 
	{
		while (vs[i] != vs.back())
		{
			serialized.append(vs[i] + ",");
		}
			
		serialized.append(vs[i]);
	}

	return serialized;
}

void Logic::DisplayParticipantList() {

//message functions
vector<char> Logic::CreateReqMessage(string rq_nbr) {
	string userTemp = "";
	string str = "REQUEST";
	str.push_back('|');
	str.append(to_string(requestCounter));
	str.push_back('|');

	//date and time
	//cout << "What date and time would you like the meeting at? " 

	//min participants

	//create participant list

	//topic

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

void Logic::HandleMessage(std::vector<char> message, sockaddr_in src_addr)
{
	// Send the message to the logger
		// Better as string or char vector? Does it even matter?

	string raw_content(message.begin(), message.end());
	int first_delim = raw_content.find_first_of('|');
	int last_delim = raw_content.find_last_of('|');

	string msg_content(message.begin(), message.begin() + last_delim + 1);
	
	if (first_delim == string::npos) {
		// Invalid message format.
	}
	else {
		string msg_type = raw_content.substr(0, first_delim);
	
		if (m_Mode == 1) // Server logic
		{
			if (msg_type == REGISTER) { // A client wishes to register for this session
				// Add them to the participants list in memory
				Logic::AddParticipant(src_addr);
				// Reply with an acknowledgement of their request
				BaseMessage ack_reg(ACK_REG, src_addr);
				m_Sender.SendUDPMessage(ack_reg.toCharVector(), src_addr);
			}
			else if (msg_type == REQ_MEET) { // A client wishes to create a meeting
				// See if there is a room available at the requested time

				int fieldCounter = 0;
				int plCounter = 0;
				vector<string> req_fields; //field[0] = RQ#
				vector<string> req_pl;

				for (int i = first_delim + 1; i < last_delim + 1; i++) {
					for (int j = i; j < last_delim + 1; j++) {
						//, only in participant list
						if (msg_content[j] == ',') {
							cout << "FOUND ," << endl;
							req_pl[plCounter] = msg_content.substr(i, j);
							plCounter++;
							i = j;
						}
						if (msg_content[j] == '|') {
							cout << "FOUND |" << endl;
							req_fields[fieldCounter] = msg_content.substr(i, j);
							cout << "FIELD FOUND: " << req_fields[fieldCounter] << endl;
							fieldCounter++;
							i = j; // Place i at the comma (it will get auto incremented past the comma)
						}
					}
				}
				// Room is unavailable
				string freeRoom = "Room1";
				if (!room1[stoi(req_fields[1])]){
					freeRoom = "Room2";
					if (!room2[stoi(req_fields[1])]) {
						freeRoom = "";
						for (int k = 0; k < req_pl.size(); k++) {
							for (int q = 0; q < s_pl.size(); q++) {
								if (req_pl[k] == s_pl[q].getClientAddr()) {
									m_Sender.SendUDPMessage(
										CreateRespMessage(req_fields[0]),
										s_pl[q].getClientSI()
									);
								}
							}
						}
					}
				}
				//Room is available
				else {
					// Build meeting object
					Meeting m (to_string(meetingCounter), freeRoom, req_fields[1], req_fields[4], SItoString(src_addr));
					meetingCounter++;
					// Start sending invitations
					for (int k = 0; k < req_pl.size(); k++) {
						for (int q = 0; q < s_pl.size(); q++) {
							if (req_pl[k] == s_pl[q].getClientAddr()) {
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

				for (int i = 0; i < participantlist.size(); i++) {
					SessionStartMsg startSession(msg_type, participantlist, participantlist[i].getClientSI());
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
				// Send an acknowledgement
				cout << "RECEIVED MESSAGE: " << msg_type << endl;
				cout << "I SHOULD REPLY!" << endl;
				cout << "Participant list: " << msg_content << endl;

				// Find all the participants
				for (int i = first_delim + 1; i < last_delim; i++) {
					if (msg_content[i] == '|') {
						break; // We reached the end of the participant list
					}
					for (int j = i; j < last_delim; j++) {
						// until you see ,
						if (msg_content[j] == ',') {
							// We've reached the next participant
							//String name = msg_content(i, j);
							//addParticipant(name);
							i = j; // Place i at the comma (it will get auto incremented past the comma)
						}
					}
				}
				lock_guard<mutex> seshlock(m_sessionMutex);
				m_sessionActive = true;
			}
			else if (msg_type == ACK_REG) { // Server has received your registration request
				// Wait for confirmation of the session start
				cout << "RECEIVED MESSAGE: " << msg_type << endl;
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

void Logic::RequestMeeting() {
	cout << "Fill out some information" << endl;
	cout << "Make sure it's valid" << endl;
	cout << "Create an object for processing by the thread" << endl;
}

int Logic::participantCount() {
	lock_guard<mutex> partilock(m_partiMutex);
	return 1;
}
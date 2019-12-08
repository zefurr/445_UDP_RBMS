#include "pch.h"
#include "Sender.h"
#include <iostream>
#include <mutex>
#include <condition_variable>

#include <string>	// Temporary for testing
#include <sstream>	// Temporary for testing

using namespace std;

Sender& Sender::getInstance() {
	static Sender _instance;
	return _instance;
}

Sender::Sender() {
	//setup the socket for sending
	// Initialize variables
	m_sockaddr_len = sizeof(m_Dest_Addr);

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &m_WSA) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");
	//create socket
	if ((m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	//setup address structure
	memset((char *)&m_Dest_Addr, 0, sizeof(m_Dest_Addr));
	m_Dest_Addr.sin_family = AF_INET;
	//si_other.sin_port = htons(PORT);
	//si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);
}

// Runs in a thread to wait for outgoing messages
// Is notified of new messages by SendUDPMessage()
// Sends message over UDP
void Sender::ProcessMessages() {
	while (m_Alive) {
		// aquire a lock on the queue
		unique_lock<mutex> lock(m_Mutex);
		// wait for notify from SendUDPMessage() function
		m_Cond_NotEmpty.wait(lock, 
			[&a = m_Alive, &mList = m_Messages]
			{ return (!a || !mList.empty()); });
			// To prevent spurious wake up make sure either 
			// we are not alive OR message list isnt empty

		// copy the message to a local var copy
		vector<BaseMessage> copy{ m_Messages };

		// clear the message list
		m_Messages.clear();
				
		// release lock
		lock.unlock();

		// send messages via the socket
		for (BaseMessage msg : copy)
		{
			strcpy_s(m_buffer, msg.toCharVector().data());
			//m_Dest_Addr.sin_port = msg.m_Destination.sin_port;
			m_Dest_Addr.sin_port = htons(m_Port - m_PortOffset);
			m_Dest_Addr.sin_addr = msg.m_Destination.sin_addr;

			//send the message
			if (sendto(m_sock, m_buffer, strlen(m_buffer), 0, (struct sockaddr *) &m_Dest_Addr, m_sockaddr_len) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}

			// FOR DEBUG OUTPUT - START
			string dest_addr;
			stringstream ss;
			ss << inet_ntoa(msg.m_Destination.sin_addr) << ":" << ntohs(msg.m_Destination.sin_port);
			dest_addr = ss.str();

			vector<char> raw_vector = msg.toCharVector();
			string msg_content(raw_vector.begin(), raw_vector.end());
			msg_content.append("|" + dest_addr);
			cout << "\nMessage sent: " << msg_content << endl;
			// FOR DEBUG OUTPUT - END
		}
	}
}

// To be called by external entities wishing to send a message
void Sender::SendUDPMessage(BaseMessage message)
{
	if (m_Alive) {
		{
			// obtain a writing lock for writing to the message queue
			lock_guard<mutex> lock(m_Mutex);
			//add message to queue
			m_Messages.push_back(message);
		}
		// notify the ProcessMessage thread of new messages
		m_Cond_NotEmpty.notify_one();
	}
}

void Sender::Startup(int port_offset)
{
	m_PortOffset = port_offset;
	m_Alive = true;
	m_ProcessingThread = new thread{ &Sender::ProcessMessages, this };
}

void Sender::Shutdown()
{
	m_Alive = false;
	m_Cond_NotEmpty.notify_one();

	if (m_ProcessingThread->joinable()) {
		m_ProcessingThread->join();
	}
	delete m_ProcessingThread; // consider checking if (m_Thread != nullptr)

	// Close the socket
}

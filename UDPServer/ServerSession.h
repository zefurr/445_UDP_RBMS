#include "pch.h"
#include "Message.h"
//#include "TimeSlot.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <stdio.h>
#include <thread>
#include <vector>
#include <mutex>

#pragma comment(lib,"ws2_32.lib")	// Winsock Library


#define BUFLEN 512					// Max length of buffer
#define PORT 8888					// The port on which to listen for incoming data
#define MAXIMUM_TIMEOUTS 3
#define TIMEOUT_SECONDS 2
#define TIMEOUT_uSECONDS 100000 // 0.1 seconds

struct ServerSession
{
private:
	ServerSession();

	SOCKET s;
	struct sockaddr_in server, si_other;
	//char buf[BUFLEN]; // define it local to each use of buffer instead
	int slen, recv_len;
	WSADATA wsa;
	struct timeval timeLimit;
	std::vector<std::thread*> m_Threads;
	std::vector<std::string> m_partyList;
	bool RegistrationMode;

	// Mutexes related to registering participants
	std::mutex orderMutex;
	unsigned int readers;
	std::mutex readersMutex;
	std::mutex exclusiveMutex;

public:
	static ServerSession& getInstance();

	void SendMessage(sockaddr_in, BaseMessage); // Send a message, no acknowledgement needed
	bool SendMessageAndAck(sockaddr_in, BaseMessage); // Send a message, wait for acknowledgement

	void AddParticipant(sockaddr_in, std::vector<char>);
	void Register();
	void StartRegistration();
	void StopRegistration();
};
#include "pch.h"
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

// definitions for messages received from clients
#define MSG_REG "REGISTER"

// definitions for messaged send by the server
#define ACK_REG "ACK_REGISTER"

struct ServerSession
{
private:
	ServerSession();

	SOCKET s;
	struct sockaddr_in server, si_other;
	int slen, recv_len;
	char buf[BUFLEN];
	WSADATA wsa;
	fd_set fdset;
	struct timeval timeLimit;
	std::vector<std::thread*> m_Threads;
	std::vector<std::string> m_participants;
	bool RegistrationMode;

	// Mutexes related to registering participants
	std::mutex orderMutex;
	unsigned int readers;
	std::mutex readersMutex;
	std::mutex exclusiveMutex;

public:
	static ServerSession& getInstance();
	void AddParticipant(sockaddr_in, std::vector<char>);
	void Register();
	void StartRegistration();
	void StopRegistration();
};
#include "pch.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <stdio.h>
#include <thread>
#include <vector>


#pragma comment(lib,"ws2_32.lib")	// Winsock Library

#define SERVER "127.0.0.1"			// ip address of udp server
#define BUFLEN 512					// Max length of buffer
#define PORT 8888					// The port on which to listen for incoming data
#define MAXIMUM_TIMEOUTS 3
#define TIMEOUT_SECONDS 2
#define TIMEOUT_uSECONDS 100000 // 0.1 seconds

// definitions for messages sent by the client
#define MSG_REG "REGISTER"
	// add as required, mirror to ServerSession.h

// definitions for messaged received from the server
#define ACK_REG "ACK_REGISTER"
	// add as required, mirror to ServerSession.h

struct ClientSession
{
private:
	ClientSession();

	struct sockaddr_in si_other;
	int s, slen = sizeof(si_other);
	char buf[BUFLEN];
	char message[BUFLEN];
	WSADATA wsa;
	fd_set fdset;
	struct timeval timeLimit;
	std::vector<std::thread*> m_Threads;
	bool RegistrationMode;

public:
	static ClientSession& getInstance();
	void Register();
	void StartRegistration();
};
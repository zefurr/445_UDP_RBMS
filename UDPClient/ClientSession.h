#include "pch.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Message.h"
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

struct ClientSession
{
private:
	ClientSession();

	struct sockaddr_in si_other;
	int s, slen = sizeof(si_other);
	char buf[BUFLEN];
	char message[BUFLEN];
	WSADATA wsa;
	struct timeval timeLimit;
	std::vector<std::thread*> m_Threads;
	bool RegistrationMode;

public:
	static ClientSession& getInstance();
	void Register();
	void StartRegistration();
};
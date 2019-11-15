#pragma once

#include "shared_winsock.h"
#include <thread>
#include "Logic.h"

//#define PORT 8888	// The port on which to listen for incoming data

class Receiver
{
public:

	void Startup(int port_offset = 0);
	void Shutdown();

	static Receiver& getInstance();

private:
	
	Logic& m_Logic = Logic::getInstance();

	SOCKET m_sock;
	struct sockaddr_in m_Receiver_Addr, m_Src_Addr;
	int m_sockaddr_len;
	WSADATA m_WSA;
	char m_buffer[BUF_LEN];

	int m_Port = 8887;


	Receiver();
	void Listen();

	std::thread* m_ListeningThread = nullptr;
	bool m_Alive;
};


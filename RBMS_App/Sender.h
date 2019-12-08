#pragma once

#include "shared_winsock.h"
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

struct Message {
	std::vector<char> content;
	sockaddr_in dest;
};

struct foo {
	int a;
	int b;
};

class Sender
{
public:

	void Startup(int port_offset = 0);
	void Shutdown();
	
	void SendUDPMessage(std::vector<char>, sockaddr_in); // Producer function

	static Sender& getInstance();

private:

	// Basic elements START
	Sender();

	SOCKET m_sock;
	struct sockaddr_in m_Dest_Addr;
	int m_sockaddr_len;
	WSADATA m_WSA;
	char m_buffer[BUF_LEN];
	int m_Port = 8888;
	int m_PortOffset = 0;

	std::thread* m_ProcessingThread = nullptr;
	bool m_Alive;
	void ProcessMessages();
	// Basic elements END

	// Consumer elements START
	std::vector<Message> m_Messages; //
	std::mutex m_Mutex;
	std::condition_variable m_Cond_NotEmpty;
	// Consumer elements END


};


#pragma once

#include "shared_winsock.h"
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Message.h"

class Sender
{
public:

	void Startup();
	void Shutdown();
	void SendUDPMessage(BaseMessage); // Producer function

	static Sender& getInstance();

private:

	// Basic elements START
	Sender();

	SOCKET m_sock;
	struct sockaddr_in m_Dest_Addr;
	int m_sockaddr_len;
	WSADATA m_WSA;
	char m_buffer[BUF_LEN];

	std::thread* m_ProcessingThread = nullptr;
	bool m_Alive;
	void ProcessMessages();
	// Basic elements END

	// Consumer elements START
	std::vector<BaseMessage> m_Messages; //
	std::mutex m_Mutex;
	std::condition_variable m_Cond_NotEmpty;
	// Consumer elements END


};


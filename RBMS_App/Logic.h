#pragma once

#include "shared_winsock.h"
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Sender.h"
#include "Message.h"

class Logic
{
public:

	static Logic& getInstance();
	void Startup(int);
	void Shutdown();
	
	void HandleMessage(std::vector<char>, sockaddr_in);

private:

	// Basic elements START
	Logic();
	std::thread* m_LogicThread = nullptr;
	int m_Mode;
	bool m_Alive;
	void MainLogic();
	// Basic elements END

	// User elements start
		// function to display the agenda
		// function to display the participant list
	// User elements start

	// Consumer elements START
	std::vector<int> m_IntMsgs; //
	std::mutex m_Mutex;
	std::condition_variable m_Cond_NotEmpty;
	// Consumer elements END

	// Producer elements START
	Sender& m_Sender = Sender::getInstance();
	// Producer elements END
};


#pragma once

#include "shared_winsock.h"
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

class Sender
{
public:

	void Startup();
	void Shutdown();
	void SendMessage(int); // Producer function

	static Sender& getInstance();

private:

	// Basic elements START
	Sender();

	std::thread* m_ProcessingThread = nullptr;
	bool m_Alive;
	void ProcessMessages();
	// Basic elements END

	// Consumer elements START
	std::vector<int> m_Messages; //
	std::mutex m_Mutex;
	std::condition_variable m_Cond_NotEmpty;
	// Consumer elements END


};


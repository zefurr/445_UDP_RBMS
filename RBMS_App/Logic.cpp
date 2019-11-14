#include "pch.h"
#include "Logic.h"

using namespace std;

Logic& Logic::getInstance() {
	static Logic _instance;
	return _instance;
}

void Logic::HandleMessage(int i)
{
	if (m_Alive) {
		{
			// obtain a writing lock for writing to the message queue
			lock_guard<mutex> lock(m_Mutex);
			//add message to queue
			m_Messages.push_back(i);
		}
		// notify the ProcessMessage thread of new messages
		m_Cond_NotEmpty.notify_one();
	}
}

Logic::Logic()
{
}

void Logic::MainLogic() {
	while (m_Alive) {
		// aquire a lock on the queue
		unique_lock<mutex> lock(m_Mutex);
		// wait for notify from SendMessage() function
		m_Cond_NotEmpty.wait(lock,
			[&a = m_Alive, &mList = m_Messages]
		{ return (!a || !mList.empty()); });
		// To prevent spurious wake up make sure either 
		// we are not alive OR message list isnt empty

	// copy the messages to a local var
		vector<int> copy{ m_Messages };

		// clear the message list
		m_Messages.clear();

		// release lock
		lock.unlock();

		// send messages via the socket
		for (int i : copy)
		{
			// TBD Read the message, do a switch case based on the type of message
			// TBD This varies greatly on whether we are a client or a server
			m_Sender.SendMessage(i);
		}
	}
	
}

void Logic::Startup()
{
	m_Alive = true;
	m_LogicThread = new thread{ &Logic::MainLogic, this };
}

void Logic::Shutdown()
{
	m_Alive = false;
	m_Cond_NotEmpty.notify_one();

	if (m_LogicThread->joinable()) {
		m_LogicThread->join();
	}
	delete m_LogicThread; // consider checking if (m_Thread != nullptr)

	// Close the socket
}


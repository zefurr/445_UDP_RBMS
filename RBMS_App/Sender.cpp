#include "pch.h"
#include "Sender.h"
#include <iostream>
#include <mutex>
#include <condition_variable>

using namespace std;

Sender& Sender::getInstance() {
	static Sender _instance;
	return _instance;
}

Sender::Sender() {
	//setup the socket for sending
}

// Runs in a thread to wait for outgoing messages
// Is notified of new messages by SendMessage()
// Sends message over UDP
void Sender::ProcessMessages() {
	while (m_Alive) {
		// aquire a lock on the queue
		unique_lock<mutex> lock(m_Mutex);
		// wait for notify from SendMessage() function
		m_Cond_NotEmpty.wait(lock, 
			[&a = m_Alive, &mList = m_Messages]
			{ return (!a || !mList.empty()); });
			// To prevent spurious wake up make sure either 
			// we are not alive OR message list isnt empty

		// copy the message to a local var
		vector<int> copy{ m_Messages };

		// clear the message list
		m_Messages.clear();
				
		// release lock
		lock.unlock();

		// send messages via the socket
		for (int i : copy)
		{
			cout << "Your message: " << i << endl;
		}
	}
}

// To be called by external entities wishing to send a message
void Sender::SendMessage(int i)
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

void Sender::Startup()
{
	m_Alive = true;
	m_ProcessingThread = new thread{ &Sender::ProcessMessages, this };
}

void Sender::Shutdown()
{
	m_Alive = false;
	m_Cond_NotEmpty.notify_one();

	if (m_ProcessingThread->joinable()) {
		m_ProcessingThread->join();
	}
	delete m_ProcessingThread; // consider checking if (m_Thread != nullptr)

	// Close the socket
}

#include "pch.h"
#include "Receiver.h"

using namespace std;

Receiver& Receiver::getInstance() {
	static Receiver _instance;
	return _instance;
}

Receiver::Receiver()
{
	// Setup socket for receiving
}

// Runs in a thread to wait for incoming messages
// Blocks while waiting to receive
// This is a producer for the Logic function
void Receiver::Listen() {
	while (m_Alive) {

		// Listen for a message
		int i = rand() % 4 + 1; // 1-5
		this_thread::sleep_for(chrono::milliseconds(1000 * i));
		// Forward the message to the logic
		m_Logic.HandleMessage(i);
	}
}

void Receiver::Startup()
{
	m_Alive = true;

	m_ListeningThread = new thread{ &Receiver::Listen, this };
}

void Receiver::Shutdown()
{
	m_Alive = false;

	// Handle the possibility of one last message

	if (m_ListeningThread->joinable()) {
		m_ListeningThread->join();
	}
	delete m_ListeningThread; // consider checking if (m_Thread != nullptr)

	// Close the socket
}

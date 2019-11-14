#pragma once

#include <thread>
#include "Logic.h"

class Receiver
{
public:

	void Startup();
	void Shutdown();

	static Receiver& getInstance();

private:
	
	Logic& m_Logic = Logic::getInstance();

	Receiver();
	void Listen();

	std::thread* m_ListeningThread = nullptr;
	bool m_Alive;
};


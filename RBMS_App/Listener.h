#pragma once

#include <thread>
#include "Logic.h"

class Listener
{
public:

	void Startup();
	void Shutdown();

	static Listener& getInstance();

private:
	
	Logic& m_Logic = Logic::getInstance();

	Listener();
	void Listen();

	std::thread* m_ListeningThread = nullptr;
	bool m_Alive;
};


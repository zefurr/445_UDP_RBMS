// RBMS_App.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Sender.h"
#include "Receiver.h"

int main() {
	using namespace std;

	Logic& logic = Logic::getInstance();
	Sender& sender = Sender::getInstance();
	Receiver& receiver = Receiver::getInstance();
	logic.Startup(); // Start logic first, it's pretty harmless on it's own
	sender.Startup(); // Start sender before receiver, otherwise we might get a message we can't reply to
	receiver.Startup(); // Start receiver third, if we get a message we may need logic and sender

	int input = '\0';
	while (input != -1) {
		cout << "Tell me something" << endl;
		cin >> input;

		logic.HandleMessage(input);
	}
	// Think about the order of shutting these down
	// If we send a message but expect a reply, we should keep the receiver open until sender thread terminates
	// If we receive a message and need to reply, we should keep the sender open until we do
	// Logic will be the one issuing these instructions, so where does it land?
	receiver.Shutdown();
	sender.Shutdown();
	logic.Shutdown();

	return 0;
}
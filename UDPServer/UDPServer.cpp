#include "pch.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Listener.h" // contains stdio.h, thread, vector, mutex

#include <iostream>
#include <string>

int main()
{
	using namespace std;
	string cmd;

	// Initial user menu // TBD any other initial options?
	cout << "Room Booking and Meeting Scheduler Server\n\n";

	cout << "Choose an option:\n";
	cout << "\t1) Begin participant registration session\n";
	cout << "\tX) Exit application\n";

	cin >> cmd; // Need to make sure we get valid input

	if (cmd == "1") {
		Listener* udpServer = Listener::getInstance();
		udpServer->StartRegistration();

		while (cmd != "END")
		{
			cout << "Enter \"END\" to stop soliciation of participants\n";
			cin >> cmd;
		}
		udpServer->StopRegistration();
	}

	// Should start a thread to handle room booking in the background around here
		// Or rather, call a function that will start a thread
		// TBD

	// Follow up menu // TBD there's probably a better way to handle this menu
	while (cmd != "X")
	{
	
		cout << "Room Booking and Meeting Scheduler Server\n\n";

		cout << "*** ROOM BOOKING SESSION IN PROGRESS ***\n";
		cout << "Choose an option:\n";
		cout << "\t1) Schedule Room Maintenance\n";
		cout << "\tX) Exit application\n";

		cin >> cmd;
	}

	// User chooses to exit application
	return 0;
}
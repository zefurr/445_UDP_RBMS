#include "pch.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "ClientSession.h" // contains stdio, thread, vector

#include <iostream>
#include <string>

int main()
{
	using namespace std;
	string cmd;

	cout << "Meeting Scheduler Client\n\n";

	cout << "Choose an option:\n";
	cout << "\t1) Register\n";
	cout << "\tX) Exit application\n";

	cin >> cmd; // Need to make sure we get valid input
	cin.clear();

	// flush the input so the newline character doesn't carry forwards
	cin.ignore(INT_MAX, '\n'); 

	ClientSession& udpClient = ClientSession::getInstance();
	udpClient.StartRegistration();
	
	while (cmd != "X")
	{

		cout << "Meeting Scheduler Client\n\n";

		cout << "Choose an option:\n";
		cout << "\t1) TBD - Meeting Request\n";
		cout << "\tX) Exit application\n";

		cin >> cmd;
	}

	return 0;
}


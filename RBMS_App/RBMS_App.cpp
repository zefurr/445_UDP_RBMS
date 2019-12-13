// RBMS_App.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <condition_variable>
//#include "Message.h"
#include "shared_winsock.h"
#include "Logic.h"
#include "Receiver.h"

#include <windows.h> // For clear_screen()

#define MIN_CLIENTS 0
#define DEBUGWAIT true

void clear_screen(char fill = ' ') {
	/*COORD tl = { 0,0 };
	CONSOLE_SCREEN_BUFFER_INFO s;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(console, &s);
	DWORD written, cells = s.dwSize.X * s.dwSize.Y;
	FillConsoleOutputCharacter(console, fill, cells, tl, &written);
	FillConsoleOutputAttribute(console, s.wAttributes, cells, tl, &written);
	SetConsoleCursorPosition(console, tl);*/
}

void debugWaiting() {
	if (DEBUGWAIT) {
		Sleep(50);
	}
}

using namespace std;

void sMainMenu(Logic& logic) {
	clear_screen();

	string input = "";
	char choice = '\0';

	while (choice != 'S' && choice != 'X') {
		debugWaiting();
		cout << "ROOM BOOKING AND MEETING SCHEDULER (RBMS)" << endl;
		cout << "***SESSION IN PROGRESS***" << endl << endl;

		cout << "Choose an action:" << endl;
		cout << "\tV) View agenda" << endl << endl;

		cout << "\tX) Exit application" << endl;

		cin >> input;
		while (!isalpha(input[0])) {
			input = "";
			cout << "Invalid input" << endl;
			cin >> input;
		}
		choice = toupper(input[0]);

		if (choice == 'V') {
			logic.DisplayAgenda();
		}
	}
}

void sRegisterMenu(Logic& logic) {
	Receiver& receiver = Receiver::getInstance();
	receiver.Startup(1);

	clear_screen();

	string input = "";
	char choice = '\0';

	while (choice != 'S' && choice != 'X') {
		debugWaiting();
		cout << "ROOM BOOKING and MEETING SCHEDULER (RBMS)" << endl << endl;

		cout << "Choose an action:" << endl;
		cout << "\tS) Start session registration" << endl << endl;

		cout << "\tX) Exit application" << endl;

		cin >> input;
		while (!isalpha(input[0])) {
			input = "";
			cout << "Invalid input" << endl;
			cin >> input;
		}
		choice = toupper(input[0]);

		if (choice == 'S') {
			debugWaiting();
			cout << "ROOM BOOKING and MEETING SCHEDULER (RBMS)" << endl << endl;

			cout << "Server accepting registration requests (-1 to end)" << endl << endl;

			int escape = '\0'; // TBD there are better ways to do this
			while (escape != -1) {
				escape = '\0';
				cin >> escape;
			}
			if (logic.participantCount() >= MIN_CLIENTS) {
				debugWaiting();

				int temp = '\0';
				cout << "Session is starting, send participants list to all clients" << endl;
				//BaseMessage start_session(SESH_START);
				//logic.HandleMessage(start_session.toCharVector()); // TBD remove message object
				logic.StartSession();

				sMainMenu(logic);
			}
			else {
				debugWaiting();
				cout << "Too few participants to start session" << endl;
			}
		}
	}
}

void cMainMenu(Logic& logic) {
	clear_screen();

	string input = "";
	char choice = '\0';

	while (choice != 'X') {
		debugWaiting();
		cout << "MEETING SCHEDULER (MS)" << endl;
		cout << "***SESSION IN PROGRESS***" << endl << endl;

		cout << "Choose an action:" << endl;
		cout << "\t1) View my agenda" << endl;
		cout << "\t2) View participant list" << endl;
		cout << "\t3) Request a meeting" << endl;
		cout << "\t4) Cancel a meeting" << endl;
		cout << "\t5) (Add) Join a meeting" << endl;
		cout << "\t6) Withdraw from a meeting" << endl << endl;

		cout << "\tX) Exit application" << endl;

		cin >> input;

		while (!isalpha(input[0]) && !isdigit(input[0])) {
			input = "";
			cout << "Invalid input" << endl;
			cin >> input;
		}
		choice = toupper(input[0]);

		switch (choice) {
			case 'X': // Exit
				break;
			case '1': // View my agenda
				logic.DisplayAgenda();
				break;
			case '2': // View participant list
				logic.DisplayParticipantList();
				break;
			case '3': // Request a meeting
				logic.RequestMeeting(logic.getMyName());
				break;
			case '4': // Cancel a meeting
				break;
			case '5': // (Add) Join a meeting
				cout << "Please specify the meeting number:" << endl;
				cin >> input;
				logic.AddToMeeting(input);
				break;
			case '6': // Withdraw from a meeting
				cout << "Please specify the meeting number:" << endl;
				cin >> input;
				logic.WithdrawFromMeeting(input);
				break;
			default:
				break;
		}
	}
}

void cRegisterMenu(Logic& logic) {
	Receiver& receiver = Receiver::getInstance();
	receiver.Startup(0);

	clear_screen();

	string input = "";
	char choice = '\0';

	while (choice != 'J' && choice != 'X') {
		debugWaiting();
		cout << "MEETING SCHEDULER (MS)" << endl << endl;

		cout << "Choose an action:" << endl;
		cout << "\tJ) Join booking session" << endl << endl;

		cout << "\tX) Exit application" << endl;

		cin >> input;
		while (!isalpha(input[0])) {
			input = "";
			cout << "Invalid input" << endl;
			cin >> input;
		}
		choice = toupper(input[0]);
	
		if (choice == 'J') {
			cout << "Sending registration request ..." << endl << endl;

			logic.JoinSession();

			int escape = '\0'; // TBD there are better ways to do this
			while (escape != -1) {
				cout << "Waiting for reply from server (-1 to end):" << endl << endl;
				escape = '\0';
				cin >> escape;
			}
			if (logic.inSession()) {
				cMainMenu(logic);
			}
		}
	}
}

void startMenu(Logic& logic) {
	clear_screen();

	string input = "";
	char choice = '\0';
	
	while (choice != 'C' && choice != 'S' && choice != 'X') {
		cout << "ROOM BOOKING and MEETING SCHEDULER (RBMS)" << endl << endl;

		cout << "Choose operating mode:" << endl;
		cout << "\tC) CLIENT" << endl;
		cout << "\tS) SERVER" << endl << endl;

		cout << "\tX) Exit application" << endl;

		cin >> input;
		while (!isalpha(input[0])) {
			input = "";
			cout << "Invalid input" << endl;
			cin >> input;
		}
		choice = toupper(input[0]);
	}

	switch (choice) {
		case 'C': // Client mode
			logic.Startup(0); // CLIENT = 0
			cRegisterMenu(logic);
			break;
		case 'S': // Server mode
			logic.Startup(1); // SERVER = 1;
			sRegisterMenu(logic);
			break;
		case 'X': // Exit
			break;
		default:
			break;
	}
}

int main() {
	
	Logic& logic = Logic::getInstance();
	startMenu(logic);
			
	cout << "Shutdown Logic..." << endl;
	logic.Shutdown();
	cout << "DONE" << endl;
	cout << "All threads have shutdown" << endl;

	return 0;
}
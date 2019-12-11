// RBMS_App.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <condition_variable>
#include "Sender.h"
#include "Receiver.h"
#include "Message.h"
#include "shared_winsock.h"

#include <windows.h> // For clear_screen()

#define SERVER "127.0.0.1"
#define PORT 8888
#define MIN_CLIENTS 0

void clear_screen(char fill = ' ') {
	COORD tl = { 0,0 };
	CONSOLE_SCREEN_BUFFER_INFO s;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(console, &s);
	DWORD written, cells = s.dwSize.X * s.dwSize.Y;
	FillConsoleOutputCharacter(console, fill, cells, tl, &written);
	FillConsoleOutputAttribute(console, s.wAttributes, cells, tl, &written);
	SetConsoleCursorPosition(console, tl);
}

using namespace std;

void sMainMenu(Logic& logic) {
	cout << "ROOM BOOKING AND MEETING SCHEDULER (RBMS)" << endl;
	cout << "***SESSION IN PROGRESS***" << endl << endl;

	cout << "Enter -1 to shutdown" << endl;

	int escape = '\0';
	while (escape != -1) {
		cin >> escape;
	}
}

void sRegisterMenu(Logic& logic) {
	clear_screen();

	string input = '\0';
	char choice = '\0';

	while (choice != 'S' && choice != 'X') {
		cout << "ROOM BOOKING and MEETING SCHEDULER (RBMS)" << endl << endl;

		cout << "Choose an action:" << endl;
		cout << "\tS) Start session registration" << endl << endl;

		cout << "\tX) Exit application" << endl;

		cin >> input;
		while (!isalpha(input[0])) {
			input = '\0';
			cout << "Invalid input" << endl;
			cin >> input;
		}
		choice = toupper(input[0]);

		if (choice == 'S') {
			cout << "ROOM BOOKING and MEETING SCHEDULER (RBMS)" << endl << endl;

			cout << "Server accepting registration requests (-1 to end)" << endl << endl;

			int escape = '\0'; // TBD there are better ways to do this
			while (escape != -1) {
				escape = '\0';
				cin >> escape;
			}
			if (logic.participantCount() >= MIN_CLIENTS) {


				int temp = '\0';
				cout << "Session is starting ..." << endl;
				BaseMessage start_session(SESH_START);
				logic.HandleMessage(start_session.toCharVector()); // TBD remove message object
				//logic.startSession();
				cout << "Sent participants list to all clients." << endl;

				sMainMenu(logic);
			}
			else {

			}
		}
	}
}

void cMainMenu(Logic& logic) {
	clear_screen();

	string input = '\0';
	char choice = '\0';

	while (choice != 'J' && choice != 'X') {
		cout << "MEETING SCHEDULER (MS)" << endl;
		cout << "***SESSION IN PROGRESS***" << endl << endl;

		cout << "Choose an action:" << endl;
		cout << "\t1) View my agenda" << endl;
		cout << "\t2) View participant list" << endl;
		cout << "\t3) Request a meeting" << endl;
		cout << "\t4) (Add) Join a meeting" << endl;
		cout << "\t5) Withdraw from a meeting" << endl;
		cout << "\t6) foo" << endl << endl;

		cout << "\tX) Exit application" << endl;

		cin >> input;

		while (!isalpha(input[0]) && !isdigit(input[0])) {
			input = '\0';
			cout << "Invalid input" << endl;
			cin >> input;
		}
		choice = toupper(input[0]);

		switch (choice) {
			case 'X': // Exit
				break;
			case '1': // View my agenda
				//logic.printAgenda();
				break;
			case '2': // View participant list
				logic.DisplayParticipantList();
				break;
			case '3': // Request a meeting
				logic.RequestMeeting();
				break;
			case '4': // (Add) Join a meeting
				break;
			case '5': // Withdraw from a meeting
				break;
			default:
		}
	}
}

void cRegisterMenu(Logic& logic) {
	clear_screen();

	string input = '\0';
	char choice = '\0';

	while (choice != 'J' && choice != 'X') {
		cout << "MEETING SCHEDULER (MS)" << endl << endl;

		cout << "Choose an action:" << endl;
		cout << "\tJ) Join booking session" << endl << endl;

		cout << "\tX) Exit application" << endl;

		cin >> input;
		while (!isalpha(input[0])) {
			input = '\0';
			cout << "Invalid input" << endl;
			cin >> input;
		}
		choice = toupper(input[0]);
	
		if (choice == 'J') {
			cout << "Sending registration request ..." << endl << endl;

			BaseMessage reg_request(REGISTER); // TBD get rid of the message object
			logic.HandleMessage(reg_request.toCharVector()); // TBD get rid of the message object

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

	string input = '\0';
	char choice = '\0';
	
	while (choice != 'C' && choice != 'S' && choice != 'X') {
		cout << "ROOM BOOKING and MEETING SCHEDULER (RBMS)" << endl << endl;

		cout << "Choose operating mode:" << endl;
		cout << "\tC) CLIENT" << endl;
		cout << "\tS) SERVER" << endl << endl;

		cout << "\tX) Exit application" << endl;

		cin >> input;
		while (!isalpha(input[0])) {
			input = '\0';
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
		default:
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
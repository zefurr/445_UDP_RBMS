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

int main() {
	using namespace std;

	sockaddr_in server_addr;
	memset((char *)&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER);

	int input = '\0';
	int temp = '\0';

	cout << "ROOM BOOKING and MEETING SCHEDULER (RBMS)" << endl << endl;

	cout << "Choose operating mode (-1 to quit):" << endl;
	cout << "\t1) CLIENT" << endl;
	cout << "\t2) SERVER" << endl << endl;

	cin >> input;

	if (input == -1) { // Quit
		return 0;
	}
	int mode = input - 1; // CLIENT = 0, SERVER = 1;

	Logic& logic = Logic::getInstance();
	Sender& sender = Sender::getInstance();
	Receiver& receiver = Receiver::getInstance();
	logic.Startup(mode); // Start logic first, it's pretty harmless on it's own
	sender.Startup(mode); // Start sender before receiver, otherwise we might get a message we can't reply to
	receiver.Startup(mode); // Start receiver third, if we get a message we may need logic and sender

	if (mode == 0) { // Client interface
		while (input != -1) {
			clear_screen();
			cout << "MEETING SCHEDULER (MS)" << endl << endl;

			cout << "Choose an action (-1 to quit):" << endl;
			cout << "\t1) Join booking session" << endl << endl;

			input = '\0';
			cin >> input;
			clear_screen();

			if (input == 1) {
				BaseMessage reg_request(REGISTER);
				cout << "Sending registration request ..." << endl << endl;
				logic.HandleMessage(reg_request.toCharVector(), server_addr);

				while (input != -1) {
				cout << "Waiting for reply from server (-1 to quit):" << endl << endl;

				input = '\0';
				cin >> input;
				
				}
				input = '\0';
			} else if (input == 2) {
				//RequestMsg meeting_request(REQ_MEET);
			}
			while (logic.inSession()) {
				clear_screen();
				cout << "MEETING SCHEDULER (MS)" << endl;
				cout << "***SESSION IN PROGRESS**" << endl << endl;

				cout << "Choose an action (-1 to quit):" << endl;
				cout << "\t1) View my agenda" << endl;
				cout << "\t2) Request a meeting " << endl;
				cout << "\t3) foo" << endl;
				cout << "\t4) foo" << endl;
				cout << "\t5) foo" << endl;
				cout << "\t6) foo" << endl << endl;

				input = '\0';
				cin >> input;
				
			}
		}
	}
	else { // Server interface
		while (input != -1) {

			cout << "ROOM BOOKING and MEETING SCHEDULER (RBMS)" << endl << endl;

			cout << "Choose an action (-1 to quit):" << endl;
			cout << "\t1) Initiate client registration mode" << endl << endl;
			cin >> input;
			clear_screen();

			if (input == 1) {
				
				while (input != -2) {
					cout << "ROOM BOOKING and MEETING SCHEDULER (RBMS)" << endl << endl;

					cout << "Registration mode activated." << endl;
					cout << "\t(-2 to end registration)" << endl << endl;
					cin >> input;
					clear_screen();
				}

				cout << "Session is starting ..." << endl;
				BaseMessage start_session(SESH_START);
				logic.HandleMessage(start_session.toCharVector(), server_addr);
				cout << "Sent participants list to all clients." << endl;
				cout << "Press a key to return to main page ..." << endl << endl;
				cin >> temp;
				clear_screen();
			}
		}
	}

	// Think about the order of shutting these down
	// If we send a message but expect a reply, we should keep the receiver open until sender thread terminates
	// If we receive a message and need to reply, we should keep the sender open until we do
	// Logic will be the one issuing these instructions, so where does it land?
	cout << "Shutdown Receiver...";
	receiver.Shutdown(); // TBD recvfrom is blocking, can't shutdown
	cout << "DONE" << endl;
	cout << "Shutdown Sender...";
	sender.Shutdown();
	cout << "DONE" << endl;
	cout << "Shutdown Logic..." << endl;
	logic.Shutdown();
	cout << "DONE" << endl;
	cout << "All threads have shutdown" << endl;

	return 0;
}
#include "pch.h"
#include "ServerSession.h" // contains stdio.h, thread, vector, mutex
#include <iostream>
#include <iterator>
#include <sstream>
#include <fstream>
#include <string>

ServerSession& ServerSession::getInstance()
{
	static ServerSession _instance;
	return _instance;
}

ServerSession::ServerSession()
{
	// Initialize variables
	slen = sizeof(si_other);
	FD_ZERO(&fdset); // for use with select()
	timeLimit.tv_sec = TIMEOUT_SECONDS;
	timeLimit.tv_usec = TIMEOUT_uSECONDS;
	RegistrationMode = false;
	readers = 0; // keep track of threads reading the participants file

	//Initialize winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	printf("Initialized.\n");
	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d\n", WSAGetLastError());
	}

	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	//Bind (use ::bind when using namespace std)
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done\n");
}

void ServerSession::AddParticipant(sockaddr_in si, std::vector<char> data)
{
	// Given data from a client and their addressing information
	// Figure out if they are already in the participant list
	// Add them if they are not

	using namespace std;
		
	string client_addr;
	stringstream ss;
	ss << inet_ntoa(si.sin_addr) << ":" << ntohs(si.sin_port);
	client_addr = ss.str();

	cout << "Received packet from " << client_addr << endl;
	copy(data.begin(), data.end(), ostream_iterator<char>(cout));
	
	// Address the third readers writers problem:
	// Any number of readers may access
	// Only one writer may access, no one may read while writing occurs
	// Once a writer requests access, allow existing readers to finish
	// Do not allow new readers until writer has finished
	
	//TBD alternative locking tools, maybe worth looking into later
	//lock_guard<mutex> lock(orderMutex);
	//std::lock_guard<std::mutex> lock(orderMutex);

	orderMutex.lock();			// Maintain the order of arrival
	readersMutex.lock();		// We will manipulate the readers counter
	if (readers == 0) {			// If we are currently no readers
		exclusiveMutex.lock();	// Get exclusive access for the readers
	}
	readers++;
	orderMutex.unlock();		// We have been served
	readersMutex.unlock();		// We are finished manipulating the readers count
	
	// Read the participant list
	bool alreadyExists = false;
	ifstream ifs("Participants.txt");
	if (ifs.is_open())
	{
		string line;
		while (!alreadyExists && getline(ifs, line))
		{
			size_t found = line.find(client_addr);
			if (found != string::npos)
			{
				// We are already in the participant list
				cout << client_addr << " already in the participant list" << endl;
				alreadyExists = true;
			}
		}
	}

	readersMutex.lock();		// We will manipulate the readers counter
	readers--;
	if (readers == 0)			// If no one is currently reading
	{
		exclusiveMutex.unlock();// Relinquish exclusive access
	}
	readersMutex.unlock();		// We are finished manipulating the readers count

	// If we were not already in the participant list, we want to write ourselves into it
	if (!alreadyExists)
	{
		orderMutex.lock();			// Maintain the order of arrival so readers dont accumulate
		exclusiveMutex.lock();		// Obtain exclusive access for writing
		orderMutex.unlock();		// We have been served

		//Add the participant to the list
		ofstream ofs("Participants.txt", ios::app);
		// using ios::app places the output at the end of the txt file
		if (ofs.is_open())
		{			
			ofs << client_addr << endl;
			ofs.close();
			//ofs.write(client_addr.c_str(), sizeof(char)*client_addr.size()); // no newline
		}

		exclusiveMutex.unlock();
	}
}

void ServerSession::Register()
{
	// Listen for incoming registration requests
	// Dispatch valid requests to be registered
	// Ignore invalid messages

	while (RegistrationMode)
	{
		printf("\nWaiting for data...\n");
		fflush(stdout);

		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);
		
		FD_SET(s, &fdset); // Monitor socket s, select will return when it is ready to be read
		// Select will return when something is available to be read on the socket, or on timeout
		select(s + 1, &fdset, NULL, NULL, &timeLimit);

		// If there is something ready to be read on the socket
		if (FD_ISSET(s, &fdset))
		{
			printf("\nMessage from UDP client: \n");

			//try to receive some data, this is a blocking call
			if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
			{
				printf("recvfrom() failed with error code : %d\n", WSAGetLastError());
				exit(EXIT_FAILURE);
			}

			// Read the data to determine if it's a valid registration request
			if (strcmp(buf, MSG_REG) == 0) // If it's valid (TBD this is kind of brute force)
			{
				// Reply to the client with an acknowledgement
				std::string ack_reg = ACK_REG;
				if (sendto(s, ack_reg.c_str(), ack_reg.length(), 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
				{
					printf("sendto() failed with error code : %d\n", WSAGetLastError());
					exit(EXIT_FAILURE);
				}

				// To avoid blocking for I/O create a thread to interact with the participant list
				std::vector<char> temp(&buf[0], &buf[BUFLEN]); // copy the buffer content into a vector so we can pass it by value
				std::thread th(&ServerSession::AddParticipant, this, si_other, temp);
				th.detach(); // Let this complete in the background || is this wise?
			}
			// Else we do nothing (unsolicited message)
		}
		// Else there was nothing to read on the socket
	}
	// Registration session terminated by user

	// TBD during development we use ports to distinguish clients and ports change when sockets change, so we dont want to close these
	closesocket(s);
	WSACleanup();
}

void ServerSession::StartRegistration()
{
	RegistrationMode = true;

	// TBD currently this is only one thread so a vector is overkill, but we might make use of this later
	m_Threads.push_back(new std::thread(&ServerSession::Register, this));
}

void ServerSession::StopRegistration()
{
	// Stop listening for new registrations
	RegistrationMode = false;

	// Join the thread(s) for registrations
	for (std::thread* t : m_Threads)
	{
		t->join();
		t->~thread();
	}

	// In pratice the participant list will be complete by now
	// In theory, its possible some of the AddParticipant threads are still running
	// Might need to let them resolve before proceeding.
		// Maybe keep track of them inside m_Threads and then do if t.joinable { t.join }
		// This would mean not detaching them


	// For each participant in the list, send the participant the full list
		// TBD
	// Wait for their acknowledgement, otherwise send it up to two more times
		// TBD - See client timeout implementation for reference
}

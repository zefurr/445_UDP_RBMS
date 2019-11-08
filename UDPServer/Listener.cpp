#include "pch.h"
#include "Listener.h" // contains stdio.h, thread, vector, mutex
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

Listener::Listener()
{
	// Initialize variables
	slen = sizeof(si_other);
	FD_ZERO(&rset);
	timeout.tv_sec = TIMEOUT_SECONDS;
	timeout.tv_usec = TIMEOUT_uSECONDS;
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

void Listener::AddParticipant(sockaddr_in si, std::vector<char> data)
{
	using namespace std;
		
	string client_addr;
	stringstream ss;
	ss << inet_ntoa(si.sin_addr) << ":" << ntohs(si.sin_port);
	client_addr = ss.str();

	cout << "Received packet from " << client_addr << endl;
	cout << "Data: " << data[0] << endl;

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

void Listener::Register()
{
	//keep listening for data
	while (RegistrationMode)
	{
		printf("\nWaiting for data...\n");
		fflush(stdout);

		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);

		FD_SET(s, &rset);
		// Select will return when something is available to be read on the socket, or on timeout
		nready = select(s, &rset, NULL, NULL, &timeout);

		// If there is something to read
		if (FD_ISSET(s, &rset))
		{
			printf("\nMessage from UDP client: \n");

			//try to receive some data, this is a blocking call
			if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
			{
				printf("recvfrom() failed with error code : %d\n", WSAGetLastError());
				exit(EXIT_FAILURE);
			}

			// We need to create a thread to handle the incoming registration message and then go right back to listening for more

			// Send si_other and data from buf to a function for generating the participant list
			std::vector<char> temp(&buf[0], &buf[BUFLEN]); // copy the buffer content into a vector so we can pass it by value
			// TBD is si_other passed by value?
			std::thread th(&Listener::AddParticipant, this, si_other, temp);
			th.detach(); // Let this complete in the background

			//now reply the client with the same data
			if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d\n", WSAGetLastError());
				exit(EXIT_FAILURE);
			}
		}

	}

	closesocket(s);
	WSACleanup();
}

void Listener::StartRegistration()
{
	RegistrationMode = true;
	m_Threads.push_back(new std::thread(&Listener::Register, this));
}

void Listener::StopRegistration()
{
	RegistrationMode = false;
	for (std::thread* t : m_Threads)
	{
		t->join();
		t->~thread();
	}

	// Send a list of all participants to each participant
	// (so they can invite people to meetings)
}

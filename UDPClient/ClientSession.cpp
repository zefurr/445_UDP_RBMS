#include "pch.h"
#include "ClientSession.h" // contains stdio.h, thread, vector, mutex
#include <iostream>
#include <iterator>
#include <sstream>
#include <fstream>
#include <string>

ClientSession& ClientSession::getInstance()
{
	static ClientSession _instance;
	return _instance;
}

ClientSession::ClientSession()
{
	// Initialize variables
	slen = sizeof(si_other);
	FD_ZERO(&fdset); // for use with select()
	timeLimit.tv_sec = TIMEOUT_SECONDS;
	timeLimit.tv_usec = TIMEOUT_uSECONDS;

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");
	//create socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	//setup address structure
	memset((char *)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);
}

void ClientSession::Register()
{
	int count_timeouts = 0;
	//start communication
	while (count_timeouts < MAXIMUM_TIMEOUTS && strcmp(buf, ACK_REG) != 0)
	{
		strcpy_s(message, MSG_REG);

		//send the message
		if (sendto(s, message, strlen(message), 0, (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		//receive a reply and print it
		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);

		FD_SET(s, &fdset); // Monitor socket s, select will return when it is ready to be read
		// Select will return when something is available to be read on the socket, or on timeout
		select(s + 1, &fdset, NULL, NULL, &timeLimit);

		// If there is something ready to be read on the socket
		if (FD_ISSET(s, &fdset))
		{
			//try to receive some data, this is a blocking call
			if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR)
			{
				printf("recvfrom() failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}
			puts(buf);
		}
		else
		{
			count_timeouts++; 
			printf("TIMEOUT - Attempts: %d\n", count_timeouts);
		}
	}
	if (count_timeouts >= MAXIMUM_TIMEOUTS)
	{
		printf("Registration FAILED after %d attempts.\n", MAXIMUM_TIMEOUTS);
	}
	else
	{
		// Now we wait for the server to send us the participants list
			// TBD
	}
	
	closesocket(s);
	WSACleanup();
}

void ClientSession::StartRegistration()
{
	printf("Attempting Registration with server at %s\n", SERVER);
	Register();
}
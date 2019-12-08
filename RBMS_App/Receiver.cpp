#include "pch.h"
#include "Receiver.h"

using namespace std;

Receiver& Receiver::getInstance() {
	static Receiver _instance;
	return _instance;
}

Receiver::Receiver()
{
	// Setup socket for receiving

	// Initialize variables
	m_sockaddr_len = sizeof(m_Src_Addr);
		//FD_ZERO(&fdset); // for use with select()
		//RegistrationMode = false;

	//Initialize winsock
	//printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &m_WSA) != 0) // use Winsock 2.2
	{
		printf("Failed. Error Code : %d\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	//printf("Initialized.\n");
	//Create a socket
	if ((m_sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d\n", WSAGetLastError());
	}
	//printf("Socket created.\n");
}

// Runs in a thread to wait for incoming messages
// Blocks while waiting to receive
// This is a producer for the Logic function
void Receiver::Listen() {
	while (m_Alive) {
		//printf("\nWaiting for data...\n");
		fflush(stdout);

		//clear the buffer by filling null, it might have previously received data
		memset(m_buffer, '\0', BUF_LEN);

		// TBD how to escape this blocking call when the server shuts down?
		// try to receive some data, this is a blocking call
		if ((recvfrom(m_sock, m_buffer, BUF_LEN, 0, (struct sockaddr *) &m_Src_Addr, &m_sockaddr_len)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d\n", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		// copy the buffer content into a vector so we can pass it by value, sockaddr_in is naturally by value
		std::vector<char> message(&m_buffer[0], &m_buffer[BUF_LEN]);
		// Pass the message to the logic, it will evaluate and process the message
		m_Logic.HandleMessage(message, m_Src_Addr);
	}
}

void Receiver::Startup(int port_offset)
{
	//Prepare the sockaddr_in structure
	m_Receiver_Addr.sin_family = AF_INET; // IPv4
	m_Receiver_Addr.sin_addr.s_addr = INADDR_ANY;
	m_Receiver_Addr.sin_port = htons(m_Port + port_offset);
	//printf("Bind failed with error code : %d\n", m_Port + port_offset);

	//Bind (use ::bind when using namespace std)
	if (::bind(m_sock, (struct sockaddr *)&m_Receiver_Addr, sizeof(m_Receiver_Addr)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	//puts("Bind done\n");

	m_Alive = true;

	m_ListeningThread = new thread{ &Receiver::Listen, this };
}

void Receiver::Shutdown()
{
	m_Alive = false;

	// Handle the possibility of one last message

	if (m_ListeningThread->joinable()) {
		m_ListeningThread->join();
	}

	// Close the socket
	closesocket(m_sock);
	WSACleanup();

	delete m_ListeningThread; // consider checking if (m_Thread != nullptr)
}

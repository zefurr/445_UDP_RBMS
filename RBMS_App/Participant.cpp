#include "pch.h"
#include "Participant.h"
#include <string>

//define static int
int Participant::clientCounter;

const std::vector<std::string> initialAvailabilities = 
{
	//e.g. 12 means timeslot 2 in room 1
	"11", "12", "13", "21", "22", "23"
};

Participant::Participant()
{
}

Participant::Participant(std::string client_addr, sockaddr_in client_si)
{
	this->client_addr = client_addr;
	this->client_si = client_si;
	this->client_name = "C" + std::to_string(this->clientCounter);
	this->availabilites = initialAvailabilities;
	this->organizer = false;

	this->clientCounter++;
}

std::string Participant::getClientName()
{
	return this->client_name;
}

std::string Participant::getClientAddr()
{
	return this->client_addr;
}

sockaddr_in Participant::getClientSI()
{
	return this->client_si;
}

void Participant::setClientAddr(std::string client_addr)
{
	this->client_addr = client_addr;
}

void Participant::setClientName(std::string name) {
	this->client_name = name;
}



//void ServerSessions::AddParticipant(sockaddr_in si, std::vector<char> data)
//{
//	// Given data from a client and their addressing information
//	// Figure out if they are already in the participant list
//	// Add them if they are not
//
//	using namespace std;
//
//	string client_addr;
//	stringstream ss;
//	ss << inet_ntoa(si.sin_addr) << ":" << ntohs(si.sin_port);
//	client_addr = ss.str();
//
//	cout << "Received packet from " << client_addr << endl;
//	copy(data.begin(), data.end(), ostream_iterator<char>(cout));
//
//	// Address the third readers writers problem:
//	// Any number of readers may access
//	// Only one writer may access, no one may read while writing occurs
//	// Once a writer requests access, allow existing readers to finish
//	// Do not allow new readers until writer has finished
//
//	//TBD alternative locking tools, maybe worth looking into later
//	//lock_guard<mutex> lock(orderMutex);
//	//std::lock_guard<std::mutex> lock(orderMutex);
//
//	orderMutex.lock();			// Maintain the order of arrival
//	readersMutex.lock();		// We will manipulate the readers counter
//	if (readers == 0) {			// If we are currently no readers
//		exclusiveMutex.lock();	// Get exclusive access for the readers
//	}
//	readers++;
//	orderMutex.unlock();		// We have been served
//	readersMutex.unlock();		// We are finished manipulating the readers count
//
//	// Read the participant list
//	bool alreadyExists = false;
//	ifstream ifs("Participants.txt");
//	if (ifs.is_open())
//	{
//		string line;
//		while (!alreadyExists && getline(ifs, line))
//		{
//			size_t found = line.find(client_addr);
//			if (found != string::npos)
//			{
//				// We are already in the participant list
//				cout << client_addr << " already in the participant list" << endl;
//				alreadyExists = true;
//			}
//		}
//	}
//
//	readersMutex.lock();		// We will manipulate the readers counter
//	readers--;
//	if (readers == 0)			// If no one is currently reading
//	{
//		exclusiveMutex.unlock();// Relinquish exclusive access
//	}
//	readersMutex.unlock();		// We are finished manipulating the readers count
//
//	// If we were not already in the participant list, we want to write ourselves into it
//	if (!alreadyExists)
//	{
//		orderMutex.lock();			// Maintain the order of arrival so readers dont accumulate
//		exclusiveMutex.lock();		// Obtain exclusive access for writing
//		orderMutex.unlock();		// We have been served
//
//		//Add the participant to the list
//		ofstream ofs("Participants.txt", ios::app);
//		// using ios::app places the output at the end of the txt file
//		if (ofs.is_open())
//		{
//			m_participants.push_back(client_addr);
//			ofs << client_addr << endl;
//			ofs.close();
//			//ofs.write(client_addr.c_str(), sizeof(char)*client_addr.size()); // no newline
//		}
//
//		exclusiveMutex.unlock();
//	}
//}

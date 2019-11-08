#include "pch.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <stdio.h>
#include <thread>
#include <vector>
#include <mutex>
//#include <condition_variable> // For semaphore class

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 512	//Max length of buffer
#define PORT 8888	//The port on which to listen for incoming data
#define TIMEOUT_SECONDS 2
#define TIMEOUT_uSECONDS 100000 // 0.1 seconds

//class Semaphore {
//public:
//	Semaphore(int count_ = 0)
//		: count(count_) {}
//
//	inline void notify()
//	{
//		std::unique_lock<std::mutex> lock(mtx);
//		count++;
//		cv.notify_one();
//	}
//
//	inline void wait()
//	{
//		std::unique_lock<std::mutex> lock(mtx);
//
//		while (count == 0) {
//			cv.wait(lock);
//		}
//		count--;
//	}
//
//	int count;
//
//private:
//	std::mutex mtx;
//	std::condition_variable cv;
//};

struct Listener
{
	SOCKET s;
	struct sockaddr_in server, si_other;
	int slen, recv_len, nready;
	char buf[BUFLEN];
	WSADATA wsa;
	fd_set rset;
	struct timeval timeout;
	std::vector<std::thread*> m_Threads;
	bool RegistrationMode;

	// Mutexes related to registering participants
	std::mutex orderMutex;
	unsigned int readers;
	std::mutex readersMutex;
	std::mutex exclusiveMutex;

	Listener();
	void AddParticipant(sockaddr_in, std::vector<char>);
	void Register();
	void StartRegistration();
	void StopRegistration();
};
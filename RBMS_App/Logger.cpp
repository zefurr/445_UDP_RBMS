#include "pch.h"
#include "Logger.h"

using namespace std;

Logger& Logger::getInstance() {
	static Logger _instance;
	return _instance;
}

Logger::Logger() {
	m_ProcessingThread = new thread{ &Logger::ProcessEntries, this };
	remove("Server_Log.txt");
	remove("Client_Log.txt");
	
	cout << "Logger instantiated" << endl;
}

void Logger::ProcessEntries() {
	while (m_SystemOnline)
	{
		// Wait to receive a message
		unique_lock<mutex> datalock(m_DataMutex);
		m_Cond_NotEmpty.wait(datalock,
			[&a = m_SystemOnline, &mList = m_Data]
		{ return (!a || !mList.empty()); });
		// To prevent spurious wakeup make sure there is data to log

// copy the data to a local var
		vector<string> copy{ m_Data };

		// clear the data
		m_Data.clear();

		// release lock
		datalock.unlock();

		// log data
		lock_guard<mutex> filelock(m_FileMutex);

		ofstream myfile(filename, ios::app);
		if (myfile.is_open())
		{
			/*myfile << "================================================================================" << endl;
			std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
			std::time_t t = std::chrono::system_clock::to_time_t(p);
			myfile << ctime(&t);
			myfile << "--------------------------------------" << endl;*/

			for (string msg : copy)
			{
				myfile << msg << endl;
				cout << msg << endl; // debug
			}
			myfile.close();
		}
		else {
			cout << "Unable to open log file";
		}
	}
}

void Logger::SetMode(int mode)
{
	if (mode == 0) {
		filename = "Client_Log.txt";
	}
	else {
		filename = "Server_Log.txt";
	}
}

//void Logger::Log(vector<string> messageToLog) {
//	lock_guard<mutex> datalock(m_DataMutex);
//	for (string& str : messageToLog) {
//		m_Data.push_back(str);
//	}
//	m_Cond_NotEmpty.notify_one();
//}

void Logger::Log(string messageToLog) {
	lock_guard<mutex> datalock(m_DataMutex);
	m_Data.push_back(messageToLog);
	m_Cond_NotEmpty.notify_one();
}

void Logger::Read() {
	lock_guard<mutex> filelock(m_FileMutex);
	string line;
	ifstream myfile(filename);
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			cout << line << '\n';
		}
		myfile.close();
	}
	else {
		cout << "Unable to open log file";
	}
}
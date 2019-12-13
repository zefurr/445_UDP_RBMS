#pragma once
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>

class Logger {
public:
	static Logger& getInstance();

	void SetMode(int);
	void Log(std::string);
	void Read();

private:
	Logger();

	bool m_SystemOnline = true;
	void ProcessEntries();
	std::string filename = "Client_Log.txt";
	std::thread* m_ProcessingThread = nullptr;
	std::vector<std::string> m_Data;
	std::mutex m_DataMutex; // For accessing the data queue
	std::mutex m_FileMutex; // For accessing the log file
	std::condition_variable m_Cond_NotEmpty;
};

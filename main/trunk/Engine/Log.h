#ifndef LOG_H_
#define LOG_H_

#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <windows.h>

using namespace std;

class Log
{
	public:
		static Log* GetInstance();
		void WriteToLogFile(DWORD deltaTime, char* functionName);

	private:
		Log(){};  // Private so that it can  not be called
		Log(Log const&){};             // copy constructor is private
		Log& operator=(Log const&){};  // assignment operator is private
		static Log* m_pInstance;
};

#endif

#ifndef LOG_H_
#define LOG_H_

#include <windows.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

using namespace std;

class Log
{
	public:
		static Log* GetInstance();
		void WriteTimedMessageToFile(DWORD deltaTime, char* message);
		void WriteTimedMessageToOutput(DWORD deltaTime, char* message);
		void WriteTextMessageToFile(char* message);
		void WriteTextMessageToOutput(char* message);

	private:
		Log();
		Log(Log const&){};
		Log& operator=(Log const&){};
		~Log();
		char* GenerateString(DWORD deltaTime, char* functionName);
		static Log* m_pInstance;
		ofstream fout;
};

#endif

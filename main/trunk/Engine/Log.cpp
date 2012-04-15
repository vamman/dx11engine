#include "log.h"

// Global static pointer used to ensure a single instance of the class.
Log* Log::m_pInstance = NULL;  
  
Log* Log::GetInstance()
{
   if (!m_pInstance)
   {
	   m_pInstance = new Log;
   }
   return m_pInstance;
}

Log::Log()
{
	remove("log.txt");
	fout.open("log.txt", ios::app);
	WriteTextMessageToFile("Log::Log");
	WriteTextMessageToOutput("Log::Log");
}

Log::~Log()
{
	fout.close();
}

void Log::WriteTimedMessageToFile(DWORD deltaTime, char* message)
{
	char resultStr[100];
	strcpy_s(resultStr, sizeof(resultStr), GenerateString(deltaTime, message));
	fout << resultStr << "\n";
}

void Log::WriteTimedMessageToOutput(DWORD deltaTime, char* message)
{
	char resultStr[100];
	strcpy_s(resultStr, sizeof(resultStr), GenerateString(deltaTime, message));
	OutputDebugStringA((LPCSTR)resultStr);
	OutputDebugStringA("\n");
}

void Log::WriteTextMessageToFile(char* message)
{
	fout << message << "\n";
}
void Log::WriteTextMessageToOutput(char* message)
{
	OutputDebugStringA((LPCSTR)message);
	OutputDebugStringA("\n");
}

char* Log::GenerateString(DWORD deltaTime, char* message)
{
	char tempString[10];
	char resultStr[100];
	strcpy_s(resultStr, message);
	_itoa_s(deltaTime, tempString, 10);
	strcat_s(resultStr, tempString);
	return resultStr;
}
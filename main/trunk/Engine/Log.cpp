#include "log.h"

// Global static pointer used to ensure a single instance of the class.
Log* Log::m_pInstance = NULL;  
  
Log* Log::GetInstance()
{
   if (!m_pInstance)   // Only allow one instance of class to be generated.
      m_pInstance = new Log;

   return m_pInstance;
}

void Log::WriteToLogFile(DWORD deltaTime, char* functionName)
{
	ofstream fout;
	char tempString[10];
	char funcName[100];

	strcpy_s(funcName, functionName);

	// Open a file to write the message to.
	fout.open("log.txt", ios::app);
	_itoa_s(deltaTime, tempString, 10);
	strcat_s(funcName, tempString);
	fout << funcName << "\n";

	fout.close();


}
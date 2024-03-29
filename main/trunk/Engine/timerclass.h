////////////////////////////////////////////////////////////////////////////////
// Filename: timerclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TIMERCLASS_H_
#define _TIMERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <windows.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: TimerClass
////////////////////////////////////////////////////////////////////////////////
class Timer
{
public:
	static Timer* GetInstance();

	bool Initialize();
	void Frame();

	float GetTime();
	void SetTimeA();
	void SetTimeB();
	DWORD GetDeltaTime();
	float GetStartTime();

private:
	Timer();
	INT64 m_frequency;
	float m_ticksPerMs;
	INT64 m_startTime;
	float m_frameTime;
	static Timer* instance;

	DWORD mStartTime;
	DWORD mTimeA;
	DWORD mTimeB;
	DWORD mDeltaTime;
};

#endif

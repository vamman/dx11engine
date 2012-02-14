///////////////////////////////////////////////////////////////////////////////
// Filename: timerclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "timerclass.h"

Timer* Timer::instance = 0;

Timer* Timer::GetInstance()
{
	if (instance == NULL)
	{
		instance = new Timer();
	}
	return instance;
}

Timer::Timer()
{
}

bool Timer::Initialize()
{
	// Check to see if this system supports high performance timers.
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	if(m_frequency == 0)
	{
		return false;
	}

	// Find out how many times the frequency counter ticks every millisecond.
	m_ticksPerMs = (float)(m_frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

	mStartTime = timeGetTime();

	return true;
}

void Timer::Frame()
{
	INT64 currentTime;
	float timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*)& currentTime);

	timeDifference = (float)(currentTime - m_startTime);

	m_frameTime = timeDifference / m_ticksPerMs;

	m_startTime = currentTime;

	return;
}

float Timer::GetTime()
{
	return m_frameTime;
}

void Timer::SetTimeA()
{
	mTimeA = timeGetTime();
}

void Timer::SetTimeB()
{
	mTimeB = timeGetTime();
}

DWORD Timer::GetDeltaTime()
{
	return mTimeB - mTimeA;
}

float Timer::GetStartTime()
{
	return (float)mStartTime;
}



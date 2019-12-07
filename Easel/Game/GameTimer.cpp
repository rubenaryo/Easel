/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/11
Description : Implementation of Timer Class
Thanks to Frank Luna [Introduction to 3D Game Programming with DirectX 11] (c) 2012
----------------------------------------------*/
#include "GameTimer.h"

using namespace System;

// Constructor fills in secondsPerCount by querying windows
GameTimer::GameTimer() :
    m_SecondsPerCount(0.0),
    m_DeltaTime(-1.0),
    m_BaseTime(0),
    m_PausedTime(0),
    m_StopTime(0),
    m_PrevTime(0),
    m_CurrTime(0),
    m_Stopped(false)
{
    // Store frequency in countsPerSec
    __int64 countsPerSec;
    QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec); 

    // Hold Seconds Per Count as a member 
    m_SecondsPerCount = 1.0 / static_cast<double>(countsPerSec);
}


float GameTimer::GameTime() const
{
    if (m_Stopped)
    {
        return (float)((m_StopTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount;
    }
    else
    {
        return (float)((m_CurrTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount;
    }
}

// Accessor for DeltaTime member
float GameTimer::DeltaTime() const
{
    return (float) m_DeltaTime;
}

void GameTimer::Reset()
{
    __int64 currTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

    m_BaseTime = currTime;
    m_PrevTime = currTime;
    m_StopTime = 0;
    m_Stopped = false;
}
void GameTimer::Start()
{
    __int64 startTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

    // When Resuming the timer from a stopped state
    if (m_Stopped)
    {
        // Accumulate paused time
        m_PausedTime += (startTime - m_StopTime);

        // Reset Previous Time
        m_PrevTime = startTime;

        // Reset Stop Time and switch flag
        m_StopTime = 0;
        m_Stopped = false;
    }
}

void GameTimer::Stop()
{
    // Only do something when not stopped
    if (m_Stopped == false)
    {
        __int64 currTime;
        QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

        // Save the time we've stopped and switch flag
        m_StopTime = currTime;
        m_Stopped = true;
    }
}

void GameTimer::Tick()
{
    // Dont run the timer if we're stopped
    if (m_Stopped)
    {
        m_DeltaTime = 0.0;
        return;
    }

    // Get the time this frame
    __int64 currTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
    m_CurrTime = currTime;

    // Find the difference between this frame and the previous one
    m_DeltaTime = (m_CurrTime - m_PrevTime) * m_SecondsPerCount;

    // Prepare for the next frame
    m_PrevTime = m_CurrTime;

    // External factors may cause mPrevTime > mCurrTime, which would result in negative deltatime. 
    // Therefore we must enforce nonnegative
    if (m_DeltaTime < 0.0)
        m_DeltaTime = 0.0;

}



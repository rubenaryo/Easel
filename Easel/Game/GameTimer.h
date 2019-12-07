/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/11
Description : Interface for Timer class
Thanks to Frank Luna [Introduction to 3D Game Programming with DirectX 11] (c) 2012
----------------------------------------------*/
#ifndef GAMETIMER_H
#define GAMETIMER_H

#include "WinApp.h"

namespace System {

    class GameTimer 
    {
    public:
        GameTimer();

        float GameTime() const;
        float DeltaTime() const;

        void Reset();
        void Start();
        void Stop();
        void Tick();
    private:
        double m_SecondsPerCount;
        double m_DeltaTime;

        __int64 m_BaseTime;
        __int64 m_PausedTime;
        __int64 m_StopTime;
        __int64 m_PrevTime;
        __int64 m_CurrTime;

        bool m_Stopped;
    };
}

#endif
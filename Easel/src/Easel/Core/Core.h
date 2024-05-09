/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2024/5
Description : Defines DLL import/export macro and other things
----------------------------------------------*/
#ifndef EASEL_CORE_H
#define EASEL_CORE_H

#if defined(ESL_PLATFORM_WINDOWS)
    #if defined(ESL_BUILD_DLL)
        #define EASEL_API __declspec(dllexport)
    #else
        #define EASEL_API __declspec(dllimport)
    #endif
#else
    #error Easel only currently support Windows!
#endif

#endif
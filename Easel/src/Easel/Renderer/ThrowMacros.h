/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : This header file defines preprocessor
helper macros for reaching certain paths
----------------------------------------------*/
#ifndef THROWMACROS_H
#define THROWMACROS_H

#include "COMException.h"
#include <Easel/Core/PathMacros.h>

// helper macro for throwing COM Exceptions. Disabled on Release Builds.
#if defined(ESL_DEBUG)
    #define COM_EXCEPT(hr) \
        if (FAILED(hr)) \
            throw Renderer::COMException(__LINE__,__FILE__, hr);
#else
    #define COM_EXCEPT(hr) hr
#endif

#endif
/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : This header file defines preprocessor
helper macros for reaching certain paths
----------------------------------------------*/
#ifndef THROWMACROS_H
#define THROWMACROS_H

#include "../../System/PathMacros.h"

// helper macro for throwing COM Exceptions 
// - requires inclusion of COMException.h
// - requires hr (HRESULT) to be defined in local scope
#define COM_EXCEPT(hr) Graphics::COMException(__LINE__,__FILE__, hr);


#endif
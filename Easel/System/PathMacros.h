/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/10
Description : This header file defines preprocessor 
helper macros for reaching certain paths 
----------------------------------------------*/
#ifndef PATHMACROS_H
#define PATHMACROS_H

// Helper macro for wide strings
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)

#define __WFILE__ WIDEN(__FILE__)
#define __WLINE__ WIDEN(__LINE__)

// Helper macros for getting correct paths. WILL ONLY WORK IN THIS PROJECT CONFIG
#define ASSETPATH "..\\Assets\\"
#define MODELPATH(fileName) (ASSETPATH ## "Models\\" ## fileName)

#endif
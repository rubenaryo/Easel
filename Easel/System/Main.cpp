/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/10
Description : This file contains the main function (entry point) for the application
----------------------------------------------*/
#include "WinApp.h"
#include "AppWindow.h"

using namespace System;

// Entry point for the application
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_  LPWSTR lpCmdLine, _In_  int nCmdShow)
{
    // On Debug Builds: Enable Runtime memory check
    #if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif 

    GameWindow window;
    DWORD style = CS_HREDRAW | CS_VREDRAW;
    DWORD ExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    if (!window.Create(L"Main Game Window", hInstance, style, ExStyle))
        return 1;

    // Show the window
    ShowWindow(window.Window(), nCmdShow);

    // Run the game
    window.RunGame();

    return 0;
}
/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Implementation of Message Loop
----------------------------------------------*/
#include "AppWindow.h"

namespace System {

    LRESULT GameWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT:
        case WM_MOVE:
        case WM_SIZE:
        case WM_ENTERSIZEMOVE:
        case WM_EXITSIZEMOVE:
        case WM_GETMINMAXINFO:
        case WM_ACTIVATEAPP:
        case WM_POWERBROADCAST:
        case WM_SYSKEYDOWN:
        case WM_MENUCHAR:
        default:
            return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
        }
        return TRUE;
    }

    // Here we invoke the window procedure to handle windows messages
    // if no message is received, we process a frame in our unique internal game pointer
    void GameWindow::RunGame()
    {
        MSG msg = {};
        while (msg.message != WM_QUIT)
        {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                //pGame->Tick();
            }
        }
    }

}
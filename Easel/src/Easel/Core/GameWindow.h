/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2024/5
Description : Game window header
----------------------------------------------*/
#ifndef EASEL_GAMEWINDOW_H
#define EASEL_GAMEWINDOW_H

#include <Easel.h>

#include "AppWindow.h"

namespace Core
{
    // A derived GameWindow class
    class EASEL_API GameWindow : public BaseWindow<GameWindow>
    {
    public:
        GameWindow();
        virtual ~GameWindow();

        // Overriden pure virtual functions from BaseWindow
        PCWSTR ClassName() const final { return L"Game Window Class"; }
        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) final;
        bool InitGame(HWND hwnd, int width, int height) override final;
        void RunGame();

    private:
        // States of the window (used to send messages to directx on resize, move, etc)
        bool m_ResizeMove = false;
        bool m_Suspended = false;
        bool m_Minimized = false;
        bool m_Fullscreen = false;

        // Heap allocated game
        Core::Game* m_pGame;
    };
}
#endif
/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Interface for central game class
This class encapsulates all app functionality
----------------------------------------------*/
#ifndef GAME_H
#define GAME_H

#include "../System/WinApp.h"
#include <memory>

namespace Game {
class Game
{
public:
    Game();
    void Init(HWND window, int width, int height);
    void Frame();

    // Callbacks for windows messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnMove();
    void OnResize(int newWidth, int newHeight);

    // Properties
    void GetDefaultSize(_Out_ int& width, _Out_ int& height);
};
}
#endif
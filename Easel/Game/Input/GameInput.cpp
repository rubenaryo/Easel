/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/11
Description : Implementation of GameInput method overrides
----------------------------------------------*/
#include "../../System/WinApp.h"
#include "InputSystem.h"
#include "GameInput.h"

namespace Input {

    GameInput::GameInput()
    {
        SetDefaultKeyMap();
    }

    GameInput::~GameInput()
    {}

    void GameInput::SetDefaultKeyMap()
    {
        m_keyMap.clear();
        m_keyMap[GameCommands::Quit] = new Chord(L"Quit", VK_ESCAPE, KeyState::JustReleased);
    }
}

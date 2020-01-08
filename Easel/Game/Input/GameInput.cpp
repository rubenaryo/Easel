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

    void GameInput::Frame()
    {
        GetInput();

        // Act on user input:
        // - Iterate through all active keys
        // - Check for commands corresponding to activated chords
        // - Do something based on those commands
        for (auto pair : m_activeKeyMap)
        {
            switch (pair.first)
            {
            case GameCommands::Quit:
                PostQuitMessage(0);
                break;
            }
        }
    }

    void GameInput::SetDefaultKeyMap()
    {
        m_keyMap.clear();
        m_keyMap[GameCommands::Quit] = new Chord(L"Quit", VK_ESCAPE, KeyState::JustReleased);
    }
}

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

    void GameInput::Frame(Graphics::Camera* a_Camera)
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

            case GameCommands::MoveForward:
                a_Camera->GetTransform()->MoveRelative(0.0f, 0.0f, 0.5f);
                break;
            case GameCommands::MoveBackward:
                break;
            case GameCommands::MoveLeft:
                break;
            case GameCommands::MoveRight:
                break;
            }
        }
    }

    void GameInput::SetDefaultKeyMap()
    {
        m_keyMap.clear();
        m_keyMap[GameCommands::Quit]         = new Chord(L"Quit", VK_ESCAPE, KeyState::JustReleased);
        m_keyMap[GameCommands::MoveForward]  = new Chord(L"Move Forward", 'W', KeyState::StillPressed);
        m_keyMap[GameCommands::MoveBackward] = new Chord(L"Move Backward", 'S', KeyState::StillPressed);
        m_keyMap[GameCommands::MoveLeft]     = new Chord(L"Move Left", 'A', KeyState::StillPressed);
        m_keyMap[GameCommands::MoveRight]    = new Chord(L"Move Right", 'D', KeyState::StillPressed);
    }
}

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

    void GameInput::Frame(float dt, Graphics::Camera* pCamera)
    {
        float speed = 5 * dt;

        // Act on user input:
        // - Iterate through all active keys
        // - Check for commands corresponding to activated chords
        // - Do something based on those commands
        for (auto pair : mActiveKeyMap)
        {
            switch (pair.first)
            {
            case GameCommands::Quit:
                PostQuitMessage(0);
                break;
            case GameCommands::MoveForward:
                pCamera->GetTransform()->MoveRelative(0.0f, 0.0f, speed);
                break;
            case GameCommands::MoveBackward:
                pCamera->GetTransform()->MoveRelative(0.0f, 0.0f, -speed);
                break;
            case GameCommands::MoveLeft:
                pCamera->GetTransform()->MoveRelative(-speed, 0.0f, 0.0f);
                break;
            case GameCommands::MoveRight:
                pCamera->GetTransform()->MoveRelative(speed, 0.0f, 0.0f);
                break;
            case GameCommands::CameraRotation:
            {
                std::pair<float, float> delta = this->GetMouseDelta();

                float mouseSensitivity = pCamera->GetSensitivity();

                delta.first *= mouseSensitivity * dt;
                delta.second *= mouseSensitivity * dt;

                
                pCamera->GetTransform()->Rotate(delta.second, delta.first, 0.0f);
                break;
            }
            }
        }
        
        GetInput();
    }

    void GameInput::SetDefaultKeyMap()
    {
        mKeyMap.clear();
        mKeyMap[GameCommands::Quit]         = new Chord(L"Quit", VK_ESCAPE, KeyState::JustReleased);
        mKeyMap[GameCommands::MoveForward]  = new Chord(L"Move Forward", 'W', KeyState::StillPressed);
        mKeyMap[GameCommands::MoveBackward] = new Chord(L"Move Backward", 'S', KeyState::StillPressed);
        mKeyMap[GameCommands::MoveLeft]     = new Chord(L"Move Left", 'A', KeyState::StillPressed);
        mKeyMap[GameCommands::MoveRight]    = new Chord(L"Move Right", 'D', KeyState::StillPressed);

        mKeyMap[GameCommands::CameraRotation] = new Chord(L"Camera Rotation", VK_RBUTTON, KeyState::StillPressed);
    }
}

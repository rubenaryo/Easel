/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Interface for central game class
This class encapsulates all app functionality
----------------------------------------------*/
#ifndef GAME_H
#define GAME_H

#include "StepTimer.h"

#include "Graphics/DeviceResources.h"
#include "Graphics/Renderer.h"

namespace Rendering
{
class Camera;
class LightingManager;
}

namespace Input
{
class GameInput;
}

namespace Game {
class Game final : public Rendering::IDeviceNotify
{
public:
    Game();
    ~Game();
    bool Init(HWND window, int width, int height);

    // Main Game Loop
    void Frame();

    // Implementation of IDeviceNotify
    // Handles sudden loss of device
    virtual void OnDeviceLost();
    virtual void OnDeviceRestored();

    // Callbacks for windows messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnMove();
    void OnResize(int newWidth, int newHeight);

    // Input Callbacks
    void OnMouseMove(short newX, short newY);

private:
    void Update(StepTimer const& timer);
    void Render();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources(int newWidth, int newHeight);

    // Application's Device Resources, such as the necessary buffers/views in video memory
    Rendering::DeviceResources* mpDeviceResources;

    // Renderer for handling smart binding of objects
    Rendering::Renderer mRenderer;

    // Lights Manager
    Rendering::LightingManager* mpLightingManager;

    // Input Management
    Input::GameInput* mpInput;

    // Main Camera
    Rendering::Camera* mpCamera;
    
    // Timer for the main game loop
    StepTimer mTimer;
};
}
#endif
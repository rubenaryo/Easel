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

#include <memory>

namespace Graphics
{
class Camera;
class LightingManager;
class Renderer;
}

namespace Input
{
class GameInput;
}

namespace Game {
class Game final : public Graphics::IDeviceNotify
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
    std::unique_ptr<Graphics::DeviceResources> mpDeviceResources;

    // Renderer for handling smart binding of objects
    std::unique_ptr<Graphics::Renderer> mpRenderer;

    // Lights Manager
    std::unique_ptr<Graphics::LightingManager> mpLightingManager;

    // Input Management
    std::unique_ptr<Input::GameInput> mpInput;

    // Main Camera
    std::unique_ptr<Graphics::Camera> mpCamera;
    
    // Timer for the main game loop
    StepTimer mTimer;
};
}
#endif
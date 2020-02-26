/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Interface for central game class
This class encapsulates all app functionality
----------------------------------------------*/
#ifndef GAME_H
#define GAME_H

#include "Graphics/D3D11App.h"
#include "Graphics/GeometryManager.h"
#include "Input/GameInput.h"
#include "Graphics/Camera.h"
#include "StepTimer.h"
#include "Graphics/DeviceResources.h"

namespace Game {
class Game final : public Graphics::IDeviceNotify
{
public:
    Game();
    void Init(HWND window, int width, int height);

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

    // Properties
    void GetDefaultSize(_Out_ int& width, _Out_ int& height);

private:
    void Update(StepTimer const& timer);
    void Render();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources(int newWidth, int newHeight);

    // Application's Device Resources, such as the necessary buffers/views in video memory
    std::unique_ptr<Graphics::DeviceResources> m_pDeviceResources;

    // Geometry Manager for the Application
    std::unique_ptr<Graphics::GeometryManager> m_pGeometryManager;

    // Input Management
    std::unique_ptr<Input::GameInput> m_pInput;

    // Main Camera
    std::unique_ptr<Graphics::Camera> m_pCamera;
    
    // Timer for the main game loop
    StepTimer m_Timer;
};
}
#endif
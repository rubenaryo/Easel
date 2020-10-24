/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Implementation of Game.h
----------------------------------------------*/
#include "Game.h"

#include "Graphics/Camera.h"
#include "Graphics/COMException.h"
#include "Graphics/LightingManager.h"
#include "Graphics/Renderer.h"

#include "Input/GameInput.h"

namespace Game {

// Initialize device resources, and link up this game to be notified of device updates
Game::Game() :
    mpDeviceResources(new Graphics::DeviceResources()),
    mpRenderer(new Graphics::Renderer()),
    mpInput(new Input::GameInput()),
    mpCamera(nullptr),
    mpLightingManager(nullptr)
{
    mpDeviceResources->RegisterDeviceNotify(this);
    mTimer.SetFixedTimeStep(false);
}

// Initialize device resource holder by creating all necessary resources
bool Game::Init(HWND window, int width, int height)
{
    // Initialize game camera
    mpCamera = new Graphics::Camera(0.0f, 0.0f, -5.0f, width / (float)height, 0.1f, 100.0f, 1.5f);

    // Grab Window handle, creates device and context
    mpDeviceResources->SetWindow(window, width, height);
    mpDeviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();
    
    // Create Devices dependent on window size
    mpDeviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources(width, height);

    auto device = mpDeviceResources->GetDevice();
    auto context = mpDeviceResources->GetContext();

    // Create Materials, Meshes, Entities
    mpRenderer->Init(mpDeviceResources);

    // Create Lights and respective cbuffers
    DirectX::XMFLOAT3A camPos;
    mpCamera->GetPosition3A(&camPos);
    mpLightingManager = new Graphics::LightingManager(mpDeviceResources->GetDevice(), camPos);

    return true;
}

// On Timer tick, run Update() on the game, then Render()
void Game::Frame()
{
    mTimer.Tick([&]()
    {
        Update(mTimer);
    });

    Render();

    mpDeviceResources->UpdateTitleBar(mTimer.GetFramesPerSecond(), mTimer.GetFrameCount());
}

void Game::Update(StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // Update the input, passing in the camera so it will update its internal information
    mpInput->Frame(elapsedTime, mpCamera);

    // Update the camera's view matrix
    mpCamera->UpdateView();

    // Update the lights (if needed)
    DirectX::XMFLOAT3A camPos;
    mpCamera->GetPosition3A(&camPos);
    mpLightingManager->Update(mpDeviceResources->GetContext(), timer.GetTotalSeconds(), camPos);
    
    // Update the renderer's view matrices, lighting information.
    mpRenderer->Update(mpDeviceResources->GetContext(), elapsedTime, mpCamera, &mpLightingManager->GetLightData());
}

void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (mTimer.GetFrameCount() == 0)
    {
        return;
    }

    // Grab pointer to device resources
    auto dr = mpDeviceResources;

    // Clear the necessary backbuffer
    dr->Clear(DirectX::Colors::Black);

    // Grab a reference to the d3d device context
    auto context = dr->GetContext();

    // Draw all geometries
    mpRenderer->Draw(context);

    // Show the new frame
    dr->Present();
}

void Game::CreateDeviceDependentResources()
{
}

void Game::CreateWindowSizeDependentResources(int newWidth, int newHeight)
{
    mpDeviceResources->WindowSizeChanged(newWidth, newHeight);
    float aspectRatio = (float)newWidth / (float)newHeight;
    mpCamera->UpdateProjection(aspectRatio);
}

Game::~Game()
{
    // Delete all unique ptrs
    delete mpLightingManager;
    delete mpCamera;
    delete mpInput;
    delete mpRenderer;
    delete mpDeviceResources;
}

#pragma region Game State Callbacks
void Game::OnDeviceLost()
{
}

void Game::OnDeviceRestored()
{
}

void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
}

void Game::OnResuming()
{
    mTimer.ResetElapsedTime();
}

// Recreates Window size dependent resources if needed
void Game::OnMove()
{
    auto r = mpDeviceResources->GetOutputSize();
    mpDeviceResources->WindowSizeChanged(r.right, r.bottom);
}

// Recreates Window size dependent resources if needed
void Game::OnResize(int newWidth, int newHeight)
{
    if (!mpDeviceResources->WindowSizeChanged(newWidth, newHeight))
        return;

    #if defined(DEBUG)
        try
        {
            CreateWindowSizeDependentResources(newWidth, newHeight);
        }
        catch (std::exception const& e)
        {
            MessageBoxA(mpDeviceResources->GetWindow(), e.what(), "Fatal Exception!", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
            DestroyWindow(mpDeviceResources->GetWindow());
        }
    #else
        CreateWindowSizeDependentResources(newWidth, newHeight);
    #endif
}

void Game::OnMouseMove(short newX, short newY)
{
    mpInput->OnMouseMove(newX, newY);
}
#pragma endregion
}


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
    mpInput(new Input::GameInput()),
    mpCamera(nullptr),
    mpLightingManager(nullptr)
{
    mDeviceResources.RegisterDeviceNotify(this);
    mTimer.SetFixedTimeStep(false);
}

// Initialize device resource holder by creating all necessary resources
bool Game::Init(HWND window, int width, int height)
{

    // Grab Window handle, creates device and context
    mDeviceResources.SetWindow(window, width, height);
    mDeviceResources.CreateDeviceResources();
    CreateDeviceDependentResources();
    
    auto device = mDeviceResources.GetDevice();
    auto context = mDeviceResources.GetContext();
    
    // Initialize game camera
    mpCamera = new Rendering::Camera(0.0f, 0.0f, -5.0f, width / (float)height, 0.1f, 100.0f, 1.5f, device, context);

    // Create Devices dependent on window size
    mDeviceResources.CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources(width, height);

    // Create Materials, Meshes, Entities
    mRenderer.Init(mDeviceResources);

    // Create Lights and respective cbuffers
    DirectX::XMFLOAT3A camPos;
    mpCamera->GetPosition3A(&camPos);
    mpLightingManager = new Rendering::LightingManager(mDeviceResources.GetDevice(), context, camPos);

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

    mDeviceResources.UpdateTitleBar(mTimer.GetFramesPerSecond(), mTimer.GetFrameCount());
}

void Game::Update(StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // Update the input, passing in the camera so it will update its internal information
    mpInput->Frame(elapsedTime, mpCamera);

    auto context = mDeviceResources.GetContext();

    // Update the camera's view matrix
    mpCamera->UpdateView(context);

    // Update the lights (if needed)
    DirectX::XMFLOAT3A camPos;
    mpCamera->GetPosition3A(&camPos);
    mpLightingManager->Update(context, timer.GetTotalSeconds(), camPos);
    
    // Update the renderer's view matrices, lighting information.
    mRenderer.Update(context, elapsedTime);
}

void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (mTimer.GetFrameCount() == 0)
    {
        return;
    }

    // Clear the necessary backbuffer
    mDeviceResources.Clear(DirectX::Colors::Black);

    // Draw all geometries
    mRenderer.Draw(mDeviceResources.GetContext());

    // Show the new frame
    mDeviceResources.Present();
}

void Game::CreateDeviceDependentResources()
{
}

void Game::CreateWindowSizeDependentResources(int newWidth, int newHeight)
{
    mDeviceResources.WindowSizeChanged(newWidth, newHeight);
    float aspectRatio = (float)newWidth / (float)newHeight;
    mpCamera->UpdateProjection(aspectRatio, mDeviceResources.GetContext());
}

Game::~Game()
{
    // Delete all unique ptrs
    delete mpLightingManager;
    delete mpCamera;
    delete mpInput;
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
    auto r = mDeviceResources.GetOutputSize();
    mDeviceResources.WindowSizeChanged(r.right, r.bottom);
}

// Recreates Window size dependent resources if needed
void Game::OnResize(int newWidth, int newHeight)
{
    if (!mDeviceResources.WindowSizeChanged(newWidth, newHeight))
        return;

    #if defined(DEBUG)
        try
        {
            CreateWindowSizeDependentResources(newWidth, newHeight);
        }
        catch (std::exception const& e)
        {
            MessageBoxA(mDeviceResources.GetWindow(), e.what(), "Fatal Exception!", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
            DestroyWindow(mDeviceResources.GetWindow());
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


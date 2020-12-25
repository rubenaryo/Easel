/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Implementation of Game.h
----------------------------------------------*/
#include "Game.h"

#include "Graphics/Camera.h"
#include "Graphics/COMException.h"
#include "Graphics/LightingManager.h"

#include "Graphics/ResourceCodex.h"

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
    using namespace Rendering;

    // Grab Window handle, creates device and context
    mDeviceResources.SetWindow(window, width, height);
    mDeviceResources.CreateDeviceResources();
    CreateDeviceDependentResources();
    
    auto device = mDeviceResources.GetDevice();
    auto context = mDeviceResources.GetContext();

    // Init all game resources
    ResourceCodex::Init(device, context);
    
    // Initialize game camera
    mpCamera = new Camera(0.0f, 0.0f, -5.0f, width / (float)height, 0.1f, 100.0f, 1.5f, device, context);

    // Create Devices dependent on window size
    mDeviceResources.CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources(width, height);

    // Create Materials, Meshes, Entities
    mEntityRenderer.Init(mDeviceResources);
    mSkyRenderer.Init(device);
    mBoard.Init(device);

    // Create Lights and respective cbuffers
    DirectX::XMFLOAT3A camPos;
    mpCamera->GetPosition3A(&camPos);
    mpLightingManager = new LightingManager(mDeviceResources.GetDevice(), context, camPos);

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
    mEntityRenderer.Update(&mDeviceResources, mpCamera->GetBindPacket(), mpLightingManager->GetBindPacket(), elapsedTime);
}

void Game::Render()
{
    auto context = mDeviceResources.GetContext();

    // Clear the necessary backbuffer
    mDeviceResources.Clear(DirectX::Colors::Black, context);

    // Draw all geometries
    mEntityRenderer.Draw(&mDeviceResources);
    mBoard.Render(context);

    // Remove Translation component from VP matrix
    mpCamera->PrepareForSkyRender(context);

    // Draw the sky, binding the appropriate rasterizer/depth states
    mDeviceResources.SetRTV(mDeviceResources.GetContext());
    context->RSSetViewports(1, &mDeviceResources.GetScreenViewport());
    Rendering::ConstantBufferUpdateManager::Bind(mpCamera->GetBindPacket(), context);
    mSkyRenderer.Draw(context);

    // Show the new frame
    mDeviceResources.Present(context);
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

    mSkyRenderer.Shutdown();
    mEntityRenderer.Shutdown();
    mBoard.Shutdown();

    Rendering::ResourceCodex::Shutdown();
    mDeviceResources.Shutdown();
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


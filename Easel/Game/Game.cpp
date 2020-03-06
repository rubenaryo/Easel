/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Implementation of Game.h
----------------------------------------------*/
#include "Game.h"

#include "Graphics/COMException.h"

namespace Game {

// Initialize unique pointer to device resources, and link up this game to be notified of device updates
Game::Game()
{
    m_pDeviceResources = std::make_unique<Graphics::DeviceResources>();
    m_pDeviceResources->RegisterDeviceNotify(this);
    
    m_pGeometryManager = std::make_unique<Graphics::GeometryManager>();
    m_pRenderer = std::make_unique<Graphics::Renderer>();

    m_pInput = std::make_unique<Input::GameInput>();

    m_Timer.SetFixedTimeStep(false);
}

// Initialize device resource holder by creating all necessary resources
bool Game::Init(HWND window, int width, int height)
{
    // Initialize game camera
    m_pCamera = std::make_unique<Graphics::Camera>(0.0f, 0.0f, -5.0f, width / (float)height, 0.1f, 100.0f, 0.8f);

    // Grab Window handle, creates device and context
    m_pDeviceResources->SetWindow(window, width, height);
    m_pDeviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();
    
    // Create Devices dependent on window size
    m_pDeviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources(width, height);

    auto device = m_pDeviceResources->GetD3DDevice();
    auto context = m_pDeviceResources->GetD3DDeviceContext();

    // Create Materials, Meshes, Entities
    m_pRenderer->Init(device);
    
    // Create Drawable Geometries
    m_pGeometryManager->Init(m_pDeviceResources.get());

    // Create Lights and respective cbuffers
    m_pLightingManager = std::make_unique<Graphics::LightingManager>(m_pDeviceResources->GetD3DDevice());

    return true;
}

// On Timer tick, run Update() on the game, then Render()
void Game::Frame()
{
    m_Timer.Tick([&]()
    {
        Update(m_Timer);
    });

    Render();

    #ifdef DEBUG 
    m_pDeviceResources->UpdateTitleBar(m_Timer.GetFramesPerSecond(), m_Timer.GetFrameCount());
    #endif
}

void Game::Update(StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // Update the input, passing in the camera so it will update its internal information
    m_pInput->Frame(elapsedTime, m_pCamera.get());

    // Update the camera's view matrix
    m_pCamera->UpdateView();

    // Update the pyramid
    m_pGeometryManager->UpdateEntities(elapsedTime);

    m_pLightingManager->Update(m_pDeviceResources->GetD3DDeviceContext(), timer.GetTotalSeconds());
}

void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_Timer.GetFrameCount() == 0)
    {
        return;
    }

    // Grab pointer to device resources
    auto dr = m_pDeviceResources.get();

    // Clear the necessary backbuffer
    dr->Clear(DirectX::Colors::CornflowerBlue);

    // Grab a reference to the d3d device context
    auto context = dr->GetD3DDeviceContext();

    // Draw all geometries
    m_pGeometryManager->DrawEntities(context, m_pCamera.get());

    // Show the new frame
    dr->Present();
}


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
    m_Timer.ResetElapsedTime();
}

// Recreates Window size dependent resources if needed
void Game::OnMove()
{
    auto r = m_pDeviceResources->GetOutputSize();
    m_pDeviceResources->WindowSizeChanged(r.right, r.bottom);
}

// Recreates Window size dependent resources if needed
void Game::OnResize(int newWidth, int newHeight)
{
    if (!m_pDeviceResources->WindowSizeChanged(newWidth, newHeight))
        return;
    try
    {
        CreateWindowSizeDependentResources(newWidth, newHeight);
    }
    catch (std::exception const& e)
    {
        MessageBoxA(m_pDeviceResources->GetWindow(), e.what(), "Fatal Exception!", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
        DestroyWindow(m_pDeviceResources->GetWindow());
    }
}

void Game::OnMouseMove(short newX, short newY)
{
    m_pInput->OnMouseMove(newX, newY);
}


void Game::CreateDeviceDependentResources()
{
}

void Game::CreateWindowSizeDependentResources(int newWidth, int newHeight)
{
    float aspectRatio = (float)newWidth / (float)newHeight;
    m_pCamera->UpdateProjection(aspectRatio);
}

Game::~Game()
{
    // Delete all unique ptrs
    m_pLightingManager.reset();
    m_pCamera.reset();
    m_pInput.reset();
    m_pGeometryManager.reset();
    m_pRenderer.reset();
    m_pDeviceResources.reset();
}

}


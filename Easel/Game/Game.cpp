/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Implementation of Game.h
----------------------------------------------*/
#include "Game.h"

namespace Game {

// Initialize unique pointer to device resources, and link up this game to be notified of device updates
Game::Game()
{
    m_pDeviceResources = std::make_unique<Graphics::DeviceResources>();
    m_pDeviceResources->RegisterDeviceNotify(this);
    
    m_pGeometryManager = std::make_unique<Graphics::GeometryManager>();
    m_pInput = std::make_unique<Input::GameInput>();
}

// Initialize device resource holder by creating all necessary resources
void Game::Init(HWND window, int width, int height)
{   
    // Initialize game camera
    m_pCamera = std::make_unique<Graphics::Camera>(0.0f, 0.0f, -5.0f, width / (float)height, 0.1f, 100.0f, 0.8f);
    
    // Create All Device Resources
    m_pDeviceResources->SetWindow(window, width, height);
    m_pDeviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_pDeviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources(width, height);

    // Create Drawable Geometries
    m_pGeometryManager->Init(m_pDeviceResources.get());

    // Bind Vertex Buffer to Pipeline

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
    m_pDeviceResources->UpdateTitleBar(m_Timer.GetFramesPerSecond());
    #endif
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

    CreateWindowSizeDependentResources(newWidth, newHeight);
}

void Game::OnMouseMove(short newX, short newY)
{
    m_pInput->OnMouseMove(newX, newY);
}

// returns the default window size (hardcoded) by parameter
void Game::GetDefaultSize(_Out_ int& width, _Out_ int& height)
{
    width = 800;
    height = 600;
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

void Game::CreateDeviceDependentResources()
{
}

void Game::CreateWindowSizeDependentResources(int newWidth, int newHeight)
{
    float aspectRatio = (float)newWidth / (float)newHeight;
    m_pCamera->UpdateProjection(aspectRatio);
}

}


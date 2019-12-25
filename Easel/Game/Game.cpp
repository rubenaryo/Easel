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
}

// Initialize device resource holder by creating all necessary resources
void Game::Init(HWND window, int width, int height)
{
    m_pDeviceResources->SetWindow(window, width, height);
    m_pDeviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_pDeviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

}

// On Timer tick, run Update() on the game, then Render()
void Game::Frame()
{
    m_Timer.Tick([&]()
    {
        Update(m_Timer);
    });

    Render();
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

    CreateWindowSizeDependentResources();
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

}

void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_Timer.GetFrameCount() == 0)
    {
        return;
    }

    // Clear the necessary backbuffer
    Clear();

    // Grab a reference to the d3d device context
    auto context = m_pDeviceResources->GetD3DDeviceContext();
    



    // Show the new frame
    m_pDeviceResources->Present();
}

// Clears all the views, then resets the viewport
void Game::Clear()
{
    auto context = m_pDeviceResources->GetD3DDeviceContext();
    auto renderTarget = m_pDeviceResources->GetRenderTargetView();
    auto depthStencil = m_pDeviceResources->GetDepthStencilView();

    // Clear all views
    context->ClearRenderTargetView(renderTarget, DirectX::Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport
    auto viewport = m_pDeviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);
}

void Game::CreateDeviceDependentResources()
{
}

void Game::CreateWindowSizeDependentResources()
{
}

}


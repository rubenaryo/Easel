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
    // Create All Device Resources
    m_pDeviceResources->SetWindow(window, width, height);
    m_pDeviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_pDeviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources(width, height);

    // Create Drawable Geometries
    m_pGeometryManager->Initialize(m_pDeviceResources.get());

    // Bind Vertex Buffer to Pipeline
    const UINT stride = sizeof(Graphics::Vertex);
    const UINT offset = 0u;
    ID3D11Buffer* vbuffer = m_pGeometryManager->GetVertexBuffer();
    m_pDeviceResources->GetD3DDeviceContext()->IASetVertexBuffers(0u, 1u, &vbuffer, &stride, &offset);
    
    // Bind Index Buffer to Pipeline
    m_pDeviceResources->GetD3DDeviceContext()->IASetIndexBuffer(m_pGeometryManager->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0u);

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
    m_pInput->Frame();

    #ifdef DEBUG
    std::wstringstream wss;
    std::pair<short, short> pt = m_pInput->GetMousePosition();
    wss << L"Cursor: {" << pt.first << L"," << pt.second << L"}  FPS:" << timer.GetFramesPerSecond();
    SetWindowText(m_pDeviceResources->GetWindow(), wss.str().c_str());
    #endif
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
    
    // Draw the triangle
    context->DrawIndexed(9u, 0u, 0u);

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

void Game::CreateWindowSizeDependentResources(int newWidth, int newHeight)
{
    float aspectRatio = (float)newWidth / (float)newHeight;
    //m_pCamera->UpdateProjection(aspectRatio);
}

}


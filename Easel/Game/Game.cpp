/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Implementation of Game.h
----------------------------------------------*/
#include "Game.h"

namespace Game {

Game::Game()
{

}

void Game::Init(HWND window, int width, int height)
{

}

void Game::Frame()
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
}

void Game::OnMove()
{
}

void Game::OnResize(int newWidth, int newHeight)
{
}

void Game::GetDefaultSize(_Out_ int& width, _Out_ int& height)
{
    width = 800;
    height = 600;
}

void Game::Update(StepTimer const& timer)
{
}

void Game::Render()
{
}

void Game::Clear()
{
}

void Game::CreateDeviceDependentResources()
{
}

void Game::CreateWindowSizeDependentResources()
{
}

}


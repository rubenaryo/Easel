/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/11
Description : Implementation of Camera Class
----------------------------------------------*/
#include "Camera.h"

namespace Graphics {
using namespace DirectX;

Camera::Camera(XMVECTOR a_Pos, XMVECTOR a_Dir, XMVECTOR a_Up, float a_Near, float a_Far) :
    m_Position(a_Pos),
    m_Direction(a_Dir),
    m_Up(a_Up),
    m_Near(a_Near),
    m_Far(a_Far)
{
    UpdateView();
    m_Projection = XMMatrixIdentity();
}

Camera::~Camera()
{}

void Camera::UpdateView()
{
    m_View = XMMatrixLookAtLH(m_Position, m_Position + m_Direction, m_Up);
}

void Camera::UpdateProjection(float a_AspectRatio)
{
    m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, a_AspectRatio, m_Near, m_Far);
}

}

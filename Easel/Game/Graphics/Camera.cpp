/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/11
Description : Implementation of Camera Class
----------------------------------------------*/
#include "Camera.h"

namespace Graphics {
using namespace DirectX;

Camera::Camera() :
    m_Position(XMVectorSet(0.f,0.f,0.f,1.f)),
    m_Forward(XMVectorSet(1.f, 0.f, 0.f, 1.f)),
    m_Target(XMVectorSet(1.f, 0.f, 0.f, 1.f)),
    m_Up(XMVectorSet(0.f, 1.f, 0.f, 1.f)),
    m_Right(XMVectorSet(0.f, 0.f, 1.f, 1.f)),
    m_fPitch(0.f),
    m_fYaw(0.f),
    m_fRoll(0.f),
    m_Far(1000.f),
    m_Near(10.f)
{
    // Grab instance of input singleton
    //m_pInput = Input::GameInput::GetInstance();

    XMMATRIX I = XMMatrixIdentity();
    XMStoreFloat4x4(&m_View, I);
    XMStoreFloat4x4(&m_Projection, I);
}

Camera::~Camera()
{}

void Camera::UpdateView()
{
    // Individual Angle Quaternions
    XMVECTOR qPitch = XMQuaternionRotationAxis(m_Right, m_fPitch);
    XMVECTOR qYaw = XMQuaternionRotationAxis(m_Up, m_fPitch);
    XMVECTOR qRoll = XMQuaternionRotationAxis(m_Forward, m_fPitch);

    // Grab how much mouse has moved this frame
    // TODO: Make this work 
    //POINTS mouseDelta = m_pInput->GetMouseDelta();

    // Orientation Quaternion
    XMVECTOR qOrientation = qPitch * qYaw;// *qRoll;
    qOrientation = XMQuaternionNormalize(qOrientation);

    XMMATRIX mat = XMMatrixRotationQuaternion(qOrientation);

    //TODO: Translation Component

    XMStoreFloat4x4(&m_View, mat);
}

void Camera::UpdateProjection(float a_AspectRatio)
{
    XMMATRIX P = XMMatrixPerspectiveFovLH(XM_PIDIV4, a_AspectRatio, m_Near, m_Far);
    XMStoreFloat4x4(&m_Projection, P);

}

}

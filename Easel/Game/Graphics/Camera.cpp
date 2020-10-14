/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/11
Description : Implementation of Camera Class
----------------------------------------------*/
#include "Camera.h"

namespace Graphics {

using namespace DirectX;

Camera::Camera(float x, float y, float z, float aspectRatio, float nearPlane, float farPlane, float sensitivity) :
    mNear(nearPlane),
    mFar(farPlane),
    mSensitivity(sensitivity),
    mForward(DirectX::XMVectorSet(0.f, 0.f, 1.f, 0.f)),
    mUp(DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f)),
    mRight(DirectX::XMVectorSet(1.f, 0.f, 0.f, 0.f))
{
    // Initialize the transform of the camera
    mPosition = XMVectorSet(x, y, z, 0);

    // Create initial matrices
    UpdateView();
    UpdateProjection(aspectRatio);
}

// Creates a new view matrix based on current position and orientation
void Camera::UpdateView()
{
    // Create view matrix
    XMMATRIX view = XMMatrixLookToLH(
        mPosition,
        mForward,
        mUp);

    // Store as a member field
    XMStoreFloat4x4(&mView, view);
}

// Updates the projection matrix (like on screen resize)
void Camera::UpdateProjection(float aspectRatio)
{
    XMMATRIX projection = XMMatrixPerspectiveFovLH(
        XM_PIDIV4,      // FOV
        aspectRatio,    // Screen Aspect ratio
        mNear,          // Near clip plane
        mFar);          // Far clip plane

    XMStoreFloat4x4(&mProjection, projection);
}

void Camera::GetPosition3A(XMFLOAT3A* out_pos) const
{
    XMStoreFloat3A(out_pos, mPosition);
}

XMVECTOR Camera::GetPosition() const
{
    return mPosition;
}

void Camera::MoveForward(float dist)
{
    MoveAlongAxis(dist, mForward);
}

void Camera::MoveRight(float dist)
{
    MoveAlongAxis(dist, mRight);
}

void Camera::MoveUp(float dist)
{
    MoveAlongAxis(dist, mUp);
}

void Camera::MoveAlongAxis(float dist, XMVECTOR axis)
{
    mPosition = XMVectorAdd(mPosition, XMVectorScale(axis, dist));
}

void Camera::Rotate(XMVECTOR quatRotation)
{
    // Calculate Forward vector
    mForward    = XMVector3Rotate(mForward, quatRotation);
    mUp         = XMVector3Rotate(mUp, quatRotation);
    mRight      = XMVector3Rotate(mRight, quatRotation);
}

}

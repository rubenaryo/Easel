/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/11
Description : Implementation of Camera Class
----------------------------------------------*/
#include "Camera.h"

namespace Graphics {

Camera::Camera(float x, float y, float z, float aspectRatio, float nearPlane, float farPlane, float sensitivity) :
    mNear(nearPlane),
    mFar(farPlane),
    mSensitivity(sensitivity)
{
    // Initialize the transform of the camera
    mTransform.SetPosition(x, y, z);

    // Create initial matrices
    UpdateView();
    UpdateProjection(aspectRatio);
}

// Creates a new view matrix based on current position and orientation
void Camera::UpdateView()
{
    using namespace DirectX;
    // Grab rotation quaternion
    XMVECTOR rotation = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&mTransform.GetPitchYawRoll()));

    // Calculate Forward vector
    XMVECTOR zDir = XMVectorSet(0, 0, 1, 0);
    XMVECTOR forward = XMVector3Rotate(zDir, rotation);

    // Create view matrix
    XMMATRIX view = XMMatrixLookToLH(
        XMLoadFloat3(&mTransform.GetPosition()),
        forward,
        XMVectorSet(0, 1, 0, 0));

    // Store as a member field
    XMStoreFloat4x4(&mView, view);
}

// Updates the projection matrix (like on screen resize)
void Camera::UpdateProjection(float a_AspectRatio)
{
    using namespace DirectX;
    XMMATRIX projection = XMMatrixPerspectiveFovLH(
        XM_PIDIV4,      // FOV
        a_AspectRatio,  // Screen Aspect ratio
        mNear,         // Near clip plane
        mFar);         // Far clip plane

    XMStoreFloat4x4(&mProjection, projection);
}

}

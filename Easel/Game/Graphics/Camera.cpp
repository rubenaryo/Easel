/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/11
Description : Implementation of Camera Class
----------------------------------------------*/
#include "Camera.h"

using namespace DirectX;
namespace Graphics {

Camera::Camera(float x, float y, float z, float a_AspectRatio, float a_Near, float a_Far, float a_Sensitivity) :
    m_Near(a_Near),
    m_Far(a_Far),
    m_Sensitivity(a_Sensitivity)
{
    // Initialize the transform of the camera
    m_Transform.SetPosition(x, y, z);

    // Create initial matrices
    UpdateView();
    UpdateProjection(a_AspectRatio);
}

// Creates a new view matrix based on current position and orientation
void Camera::UpdateView()
{
    // Grab rotation quaternion
    XMVECTOR rotation = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&m_Transform.GetPitchYawRoll()));

    // Calculate Forward vector
    XMVECTOR zDir = XMVectorSet(0, 0, 1, 0);
    XMVECTOR forward = XMVector3Rotate(zDir, rotation);

    // Create view matrix
    XMMATRIX view = XMMatrixLookToLH(
        XMLoadFloat3(&m_Transform.GetPosition()),
        forward,
        XMVectorSet(0, 1, 0, 0));

    // Store as a member field
    XMStoreFloat4x4(&m_View, view);
}

// Updates the projection matrix (like on screen resize)
void Camera::UpdateProjection(float a_AspectRatio)
{
    XMMATRIX projection = XMMatrixPerspectiveFovLH(
        XM_PIDIV4,      // FOV
        a_AspectRatio,  // Screen Aspect ratio
        m_Near,         // Near clip plane
        m_Far);         // Far clip plane

    XMStoreFloat4x4(&m_Projection, projection);
}

}

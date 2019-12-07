/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/11
Description : Interface for Quaternion-Based Camera functionality 
----------------------------------------------*/
#ifndef CAMERA_H
#define CAMERA_H
#include "../Input/GameInput.h"
#include <DirectXMath.h>

namespace Graphics {    
using namespace DirectX;

class Camera
{
public:
    Camera();
    ~Camera();

private:
    // Updates Camera's View Matrix
    void UpdateView();

    // Updates Camera's Projection Matrix
    void UpdateProjection(float a_AspectRatio);

private:
    // Position + Local Axis Vectors
    XMVECTOR m_Position;
    XMVECTOR m_Forward;
    XMVECTOR m_Target;
    XMVECTOR m_Up;
    XMVECTOR m_Right;

    // View and Projection Matrices
    XMFLOAT4X4 m_View;
    XMFLOAT4X4 m_Projection;

    // Euler Angles used to compute rotation quaternions (Maybe)
    float m_fYaw;
    float m_fPitch;
    float m_fRoll;

    // Position of near and far planes along forward axis
    float m_Near;
    float m_Far;

};
    
}


#endif
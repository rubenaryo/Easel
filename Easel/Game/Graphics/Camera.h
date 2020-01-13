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
    Camera(XMVECTOR a_Pos, XMVECTOR a_Dir, XMVECTOR a_Up, float a_Near, float a_Far);
    ~Camera();

public:
    // Updates Camera's View Matrix
    void UpdateView();

    // Updates Camera's Projection Matrix
    void UpdateProjection(float a_AspectRatio);

private:
    // Position + Local Axis Vectors
    XMVECTOR m_Position;
    XMVECTOR m_Direction;
    XMVECTOR m_Up;

    // View and Projection Matrices
    XMMATRIX m_View;
    XMMATRIX m_Projection;

    // Position of near and far planes along forward axis
    float m_Near;
    float m_Far;

};
    
}


#endif
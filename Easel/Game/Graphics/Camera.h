/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/11
Description : Interface for Quaternion-Based Camera functionality 
----------------------------------------------*/
#ifndef CAMERA_H
#define CAMERA_H
#include "../Transform.h"
#include "D3D11App.h"

namespace Graphics {    
using namespace DirectX;

class Camera
{
public:
    Camera(float x, float y, float z, float a_AspectRatio, float a_Near, float a_Far, float a_Sensitivity);
    Camera() = delete;
    ~Camera() = default;

public:
    // Main Update Method, runs per frame
    void Update(float dt);

    // Updates Camera's View Matrix
    void UpdateView();

    // Updates Camera's Projection Matrix
    void UpdateProjection(float a_AspectRatio);

    // Accessors:
    Game::Transform* GetTransform()   { return &m_Transform; }
    XMFLOAT4X4 GetView()        { return m_View;       }
    XMFLOAT4X4 GetProjection()  { return m_Projection; }

private:
    // Information about camera position and rotation
    Game::Transform m_Transform;

    // View and Projection Matrices
    XMFLOAT4X4 m_View;
    XMFLOAT4X4 m_Projection;

    // Position of near and far planes along forward axis
    float m_Near;
    float m_Far;

    // Look Sensitivity
    float m_Sensitivity;
};
    
}


#endif
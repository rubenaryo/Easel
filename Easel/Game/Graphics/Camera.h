/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/11
Description : Interface for Quaternion-Based Camera functionality 
----------------------------------------------*/
#ifndef CAMERA_H
#define CAMERA_H

#include "CBufferStructs.h"
#include "DXCore.h"

#include "../Transform.h"

namespace Input
{
class GameInput;
}

namespace Graphics {

class Camera
{
friend class Input::GameInput;

public:
    Camera(float x, float y, float z, float aspectRatio, float nearPlane, float farPlane, float sensitivity);
    Camera() = delete;
    ~Camera() = default;

public:
    // Updates Camera's View Matrix
    void UpdateView();

    // Updates Camera's Projection Matrix
    void UpdateProjection(float aspectRatio);

    // Accessors:
    const cbCamera      AsConstantBuffer()  const  { return cbCamera{mView, mProjection}; }

    DirectX::XMFLOAT4X4 GetView()           const  { return mView;         }
    DirectX::XMFLOAT4X4 GetProjection()     const  { return mProjection;   }
    float               GetSensitivity()    const  { return mSensitivity;  }
    
    void GetPosition3A(DirectX::XMFLOAT3A* out_pos) const;
    DirectX::XMVECTOR   GetPosition() const;

private:
    // View and Projection Matrices
    DirectX::XMFLOAT4X4 mView;
    DirectX::XMFLOAT4X4 mProjection;

    // Camera's local axis and position
    DirectX::XMVECTOR   mForward;
    DirectX::XMVECTOR   mRight;
    DirectX::XMVECTOR   mUp;
    DirectX::XMVECTOR   mPosition;

    // Position of near and far planes along forward axis
    float mNear;
    float mFar;

    // Look Sensitivity
    float mSensitivity;

private: // For GameInput only
    void MoveForward(float dist);
    void MoveRight(float dist);
    void MoveUp(float dist);
    void MoveAlongAxis(float dist, DirectX::XMVECTOR axis); // Assumes normalized axis
    void Rotate(DirectX::XMVECTOR quatRotation);
};
}


#endif
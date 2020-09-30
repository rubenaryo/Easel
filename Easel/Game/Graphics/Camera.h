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

namespace Graphics {

class Camera
{
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
    const cbCamera      AsConstantBuffer() const { return cbCamera{mView, mProjection}; }
    Game::Transform*    GetTransform()           { return &mTransform;   }
    DirectX::XMFLOAT4X4 GetView() const          { return mView;         }
    DirectX::XMFLOAT4X4 GetProjection() const    { return mProjection;   }
    float               GetSensitivity() const   { return mSensitivity;  }

private:
    // Information about camera position and rotation
    Game::Transform mTransform;

    // View and Projection Matrices
    DirectX::XMFLOAT4X4 mView;
    DirectX::XMFLOAT4X4 mProjection;

    // Position of near and far planes along forward axis
    float mNear;
    float mFar;

    // Look Sensitivity
    float mSensitivity;
};
}


#endif
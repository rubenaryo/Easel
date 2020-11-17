/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/11
Description : Interface for Quaternion-Based Camera functionality 
----------------------------------------------*/
#ifndef CAMERA_H
#define CAMERA_H

#include "CBufferStructs.h"
#include "ConstantBuffer.h"
#include "DXCore.h"

namespace Input
{
class GameInput;
}

namespace Rendering {

class Camera
{
friend class Input::GameInput;

public:
    Camera(float x, float y, float z, float aspectRatio, float nearPlane, float farPlane, float sensitivity, ID3D11Device* device, ID3D11DeviceContext* context);
    Camera() = delete;
    ~Camera();

public:
    // Updates Camera's View Matrix
    void UpdateView(ID3D11DeviceContext* context);

    // Removes the translation from the view matrix, and then updates the view-projection constant buffer
    void PrepareForSkyRender(ID3D11DeviceContext* context);

    // Updates Camera's Projection Matrix
    void UpdateProjection(float aspectRatio, ID3D11DeviceContext* context);

    DirectX::XMMATRIX   GetView()           const  { return mView;         }
    DirectX::XMMATRIX   GetProjection()     const  { return mProjection;   }
    float               GetSensitivity()    const  { return mSensitivity;  }
    
    void GetPosition3A(DirectX::XMFLOAT3A* out_pos) const;
    DirectX::XMVECTOR   GetPosition() const;

private:
    // View and Projection Matrices
    DirectX::XMMATRIX   mView;
    DirectX::XMMATRIX   mProjection;
    DirectX::XMFLOAT4X4 mViewProjection;

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

    // Bindable
    ConstantBufferBindPacket mBindPacket;

private: // For GameInput only
    void MoveForward(float dist);
    void MoveRight(float dist);
    void MoveUp(float dist);
    void MoveAlongAxis(float dist, DirectX::XMVECTOR axis); // Assumes normalized axis
    void Rotate(DirectX::XMVECTOR quatRotation);
    
    void UpdateConstantBuffer(ID3D11DeviceContext* context);
};
}


#endif
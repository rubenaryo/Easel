/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Transform class for game objects
----------------------------------------------*/
#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "Graphics/DXCore.h"

namespace Game {

class Transform
{
public:
    Transform();

    DirectX::XMFLOAT4X4 GetWorldMatrix();
    DirectX::XMFLOAT3   GetPosition()       const;
    DirectX::XMFLOAT3   GetScale()          const;
    DirectX::XMFLOAT3   GetPitchYawRoll()   const;

    // Setters
    void SetPosition(float x, float y, float z);
    void SetScale(float x, float y, float z);
    void SetRotation(float pitch, float yaw, float roll);

    // Transformers
    void MoveAbsolute(float x, float y, float z);
    void Rotate(float pitch, float yaw, float roll);
    void Scale(float x, float y, float z);
    void MoveRelative(float x, float y, float z);

private:

    bool m_MatrixDirty = true;

    // Information about world position
    DirectX::XMFLOAT4X4  m_World;
    DirectX::XMFLOAT3    m_Position;
    DirectX::XMFLOAT3    m_Scale;
    DirectX::XMFLOAT3    m_PitchYawRoll;
};
}
#endif
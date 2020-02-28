/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Transform class for game objects
----------------------------------------------*/
#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "Graphics/D3D11App.h"

namespace Game {
using DirectX::XMFLOAT4X4;
using DirectX::XMFLOAT3;

class Transform
{
public:
    Transform();

    XMFLOAT4X4 GetWorldMatrix();
    XMFLOAT3   GetPosition() const;
    XMFLOAT3   GetScale() const;
    XMFLOAT3   GetPitchYawRoll() const;
    bool       NeedsUpdate() const noexcept { return m_MatrixDirty; }

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
    XMFLOAT4X4  m_World;
    XMFLOAT3    m_Position;
    XMFLOAT3    m_Scale;
    XMFLOAT3    m_PitchYawRoll;
};
}
#endif
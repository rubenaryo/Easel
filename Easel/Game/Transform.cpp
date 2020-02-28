/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Transform class implementation
----------------------------------------------*/
#include "Transform.h"

namespace Game {
using namespace DirectX;

Transform::Transform() :
    m_Position(0.0f, 0.0f, 0.0f),
    m_Scale(1.0f, 1.0f, 1.0f),
    m_PitchYawRoll(0.0f, 0.0f, 0.0f),
    m_MatrixDirty(false)
{
    XMStoreFloat4x4(&m_World, XMMatrixIdentity());
}

void Transform::MoveAbsolute(float x, float y, float z)
{
    m_Position.x += x;
    m_Position.y += y;
    m_Position.z += z;
    m_MatrixDirty = true;
}

void Transform::MoveRelative(float x, float y, float z)
{
    XMVECTOR movement = XMVectorSet(x, y, z, 0);
    XMVECTOR rotation = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&m_PitchYawRoll));

    // Rotate by the quaternion
    XMVECTOR dir = XMVector3Rotate(movement, rotation);

    // Store as member field
    XMStoreFloat3(&m_Position, XMLoadFloat3(&m_Position) + dir);
}

void Transform::Rotate(float p, float y, float r)
{
    m_PitchYawRoll.x += p;
    m_PitchYawRoll.y += y;
    m_PitchYawRoll.z += r;
    m_MatrixDirty = true;
}

void Transform::Scale(float x, float y, float z)
{
    m_Scale.x *= x;
    m_Scale.y *= y;
    m_Scale.z *= z;
    m_MatrixDirty = true;
}

void Transform::SetPosition(float x, float y, float z)
{
    m_Position.x = x;
    m_Position.y = y;
    m_Position.z = z;
    m_MatrixDirty = true;
}

void Transform::SetRotation(float p, float y, float r)
{
    m_PitchYawRoll.x = p;
    m_PitchYawRoll.y = y;
    m_PitchYawRoll.z = r;
    m_MatrixDirty = true;
}

void Transform::SetScale(float x, float y, float z)
{
    m_Scale.x = x;
    m_Scale.y = y;
    m_Scale.z = z;
    m_MatrixDirty = true;
}

DirectX::XMFLOAT3 Transform::GetPosition() const { return m_Position; }

DirectX::XMFLOAT3 Transform::GetPitchYawRoll() const { return m_PitchYawRoll; }

DirectX::XMFLOAT3 Transform::GetScale() const { return m_Scale; }

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
    if (m_MatrixDirty) // Recalculate World Matrix?
    {
        // Calculate individual pieces
        XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&m_Position));
        XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_PitchYawRoll));
        XMMATRIX scaling = XMMatrixScalingFromVector(XMLoadFloat3(&m_Scale));

        // Calculate world matrix
        XMMATRIX worldmat = scaling * rotation * translation;
        XMStoreFloat4x4(&m_World, worldmat);

        // Switch flag
        m_MatrixDirty = false;
    }

    return m_World;
}
}
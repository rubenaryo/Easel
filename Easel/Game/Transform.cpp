/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Transform class implementation
----------------------------------------------*/
#include "Transform.h"

namespace Game {

Transform::Transform() :
    mPosition(0.0f, 0.0f, 0.0f),
    mScale(1.0f, 1.0f, 1.0f),
    mPitchYawRoll(0.0f, 0.0f, 0.0f),
    mMatrixDirty(false)
{
    XMStoreFloat4x4(&mWorld, DirectX::XMMatrixIdentity());
}

void Transform::MoveAbsolute(float x, float y, float z)
{
    mPosition.x += x;
    mPosition.y += y;
    mPosition.z += z;
    mMatrixDirty = true;
}

void Transform::MoveRelative(float x, float y, float z)
{
    using namespace DirectX;
    XMVECTOR movement = XMVectorSet(x, y, z, 0);
    XMVECTOR rotation = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&mPitchYawRoll));

    // Rotate by the quaternion
    XMVECTOR dir = DirectX::XMVector3Rotate(movement, rotation);

    // Store as member field
    XMStoreFloat3(&mPosition, XMLoadFloat3(&mPosition) + dir);
}

void Transform::Rotate(float p, float y, float r)
{
    mPitchYawRoll.x += p;
    mPitchYawRoll.y += y;
    mPitchYawRoll.z += r;
    mMatrixDirty = true;
}

void Transform::Scale(float x, float y, float z)
{
    mScale.x *= x;
    mScale.y *= y;
    mScale.z *= z;
    mMatrixDirty = true;
}

void Transform::SetPosition(float x, float y, float z)
{
    mPosition.x = x;
    mPosition.y = y;
    mPosition.z = z;
    mMatrixDirty = true;
}

void Transform::SetRotation(float p, float y, float r)
{
    mPitchYawRoll.x = p;
    mPitchYawRoll.y = y;
    mPitchYawRoll.z = r;
    mMatrixDirty = true;
}

void Transform::SetScale(float x, float y, float z)
{
    mScale.x = x;
    mScale.y = y;
    mScale.z = z;
    mMatrixDirty = true;
}

DirectX::XMFLOAT3 Transform::GetPosition() const { return mPosition; }

DirectX::XMFLOAT3 Transform::GetPitchYawRoll() const { return mPitchYawRoll; }

DirectX::XMFLOAT3 Transform::GetScale() const { return mScale; }

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
    using namespace DirectX;
    
    if (mMatrixDirty) // Recalculate World Matrix?
    {
        
        // Calculate individual pieces
        XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&mPosition));
        XMMATRIX rotation    = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&mPitchYawRoll));
        XMMATRIX scaling     = XMMatrixScalingFromVector(XMLoadFloat3(&mScale));

        // Calculate world matrix
        XMMATRIX worldmat = scaling * rotation * translation;
        XMStoreFloat4x4(&mWorld, worldmat);

        // Switch flag
        mMatrixDirty = false;
    }

    return mWorld;
}
}
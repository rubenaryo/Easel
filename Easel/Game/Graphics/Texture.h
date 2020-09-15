/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Wrapper for a Bindable texture object,
which is held in a collection by materials
----------------------------------------------*/
#ifndef TEXTURE_H
#define TEXTURE_H

#include "DXCore.h"
#include "IBindable.h"

namespace Graphics {

class Texture : IBindable
{
public:
    // The value of its 'Type' is also the slot to which the resource will be bound
    enum class Type
    {
        DIFFUSE   = 0U,
        NORMAL    = 1U,
        SPECULAR  = 2U,
        ROUGHNESS = 3U,
        CUBE      = 4U
    };

    Texture(ID3D11ShaderResourceView* srv, Type type) :
        mpSRV(srv), mType(type)
    {}

    ~Texture() { mpSRV->Release(); }

    // Binds SRV to 'mType' slot
    void Bind(ID3D11DeviceContext* context) const override
    {
        context->PSSetShaderResources(static_cast<UINT>(mType), 1u, &mpSRV);
    }

    const Type& GetType() const noexcept
    {
        return mType;
    }

    ID3D11ShaderResourceView* GetSRV() const noexcept
    {
        return mpSRV;
    }

private:
    ID3D11ShaderResourceView* mpSRV;
    UINT mSlot;
    Type mType;

public: // Disable other constructors
    Texture() = delete;
    Texture(Texture const&) = delete;
};

}


#endif
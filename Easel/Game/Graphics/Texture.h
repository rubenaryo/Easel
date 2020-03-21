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
        DIFFUSE   = 0u,
        NORMAL    = 1u,
        SPECULAR  = 2u,
        ROUGHNESS = 3u
    };

    Texture(ID3D11ShaderResourceView* srv, Type type) :
        m_pSRV(srv), m_Type(type)
    {}

    ~Texture() { m_pSRV->Release(); }

    // Binds SRV to 'm_Type' slot
    void Bind(ID3D11DeviceContext* context) noexcept override
    {
        context->PSSetShaderResources(static_cast<UINT>(m_Type), 1u, &m_pSRV);
    }

private:
    ID3D11ShaderResourceView* m_pSRV;
    Type m_Type;

public: // Disable other constructors
    Texture() = delete;
    Texture(Texture const&) = delete;
};

}


#endif
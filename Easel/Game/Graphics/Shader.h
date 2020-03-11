/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Wrapper for Vertex/Pixel/other shader code
----------------------------------------------*/
#ifndef SHADER_H
#define SHADER_H

#include "DXCore.h"
#include "IBindable.h"
#include "ConstantBuffer.h"
#include "CBufferStructs.h"
#include <string>
#include <vector>

#include <wrl/client.h> // ComPtr

namespace Graphics {

// TODO: Make a generalized "Shader" class cause there's WAY too much repeat code in these two
// Not good for when I get into domain, hull, or other shaders
class VertexShader : public IBindable
{
public:
    VertexShader(const wchar_t* path, ID3D11Device* device);
    ~VertexShader();

    void Bind(ID3D11DeviceContext* context) noexcept override;

    // Add the constant buffer to one of the 14 possible registers
    void AddConstantBuffer(VSConstantBuffer* pCBuffer);

    // Sets the data for the constant buffer in a designated slot
    // return false if slot is out of bounds
    bool SetBufferData(ID3D11DeviceContext* context, UINT slot, UINT size, void* pData);

    // Getter for internal shader
     ID3D11VertexShader* GetShader();

private:
    std::wstring m_Path;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>  m_pVertexShader;
    VSConstantBuffer*                           m_pCBuffers[14];

public:
    VertexShader()                                  = delete;
    VertexShader(const VertexShader&)               = delete;
    VertexShader& operator=(const VertexShader&)    = delete;
};

class PixelShader : public IBindable
{
public:
    PixelShader(const wchar_t* path, ID3D11Device* device);
    ~PixelShader();

    void Bind(ID3D11DeviceContext* context) noexcept override;

     // Add the constant buffer to one of 14 possible registers
    void AddConstantBuffer(PSConstantBuffer* pCBuffer);

    // Sets the data for the constant buffer in a designated slot
    // return false if slot is out of bounds
    bool SetBufferData(ID3D11DeviceContext* context, UINT slot, UINT size, void* pData);

    // Getter for internal shader
     ID3D11PixelShader* GetShader();
private:
    std::wstring m_Path;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   m_pPixelShader;
    PSConstantBuffer*                           m_pCBuffers[14];

public:
    PixelShader()                                 = delete;
    PixelShader(const PixelShader&)               = delete;
    PixelShader& operator=(const PixelShader&)    = delete;
};

}


#endif
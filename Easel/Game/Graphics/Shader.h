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

class VertexShader : public IBindable
{
public:
    VertexShader(const wchar_t* path, ID3D11Device* device);
    ~VertexShader() = default;

    void Bind(ID3D11DeviceContext* context) noexcept override;

    // Returns the index (slot) within internal vector to which the buffer was added
    // Note: the order in which this function is called must match the order of the constant buffers HLSL-side
    UINT AddConstantBuffer(VSConstantBuffer* pCBuffer);

    // Sets the data for the constant buffer in a designated slot
    // return false if slot is out of bounds
    bool SetBufferData(ID3D11DeviceContext* context, UINT slot, UINT size, void* pData);

    // Getter for internal shader
     ID3D11VertexShader* GetShader();

private:
    std::wstring m_Path;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>  m_pVertexShader;
    std::vector<VSConstantBuffer*>              m_pCBuffers;

public:
    VertexShader()                                  = delete;
    VertexShader(const VertexShader&)               = delete;
    VertexShader& operator=(const VertexShader&)    = delete;
};

class PixelShader : public IBindable
{
public:
    PixelShader(const wchar_t* path, ID3D11Device* device);
    ~PixelShader() = default;

    void Bind(ID3D11DeviceContext* context) noexcept override;

    // Returns the index (slot) within internal vector to which the buffer was added
    // Note: the order in which this function is called must match the order of the constant buffers HLSL-side
     UINT AddConstantBuffer(PSConstantBuffer* pCBuffer);

    // Sets the data for the constant buffer in a designated slot
    // return false if slot is out of bounds
    bool SetBufferData(ID3D11DeviceContext* context, UINT slot, UINT size, void* pData);

    // Getter for internal shader
     ID3D11PixelShader* GetShader();
private:
    std::wstring m_Path;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   m_pPixelShader;
    std::vector<PSConstantBuffer*>              m_pCBuffers;

public:
    PixelShader()                                 = delete;
    PixelShader(const PixelShader&)               = delete;
    PixelShader& operator=(const PixelShader&)    = delete;
};

}


#endif
/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Interface for management of geometry 
buffers, input layouts, shader compilation
----------------------------------------------*/
#ifndef GEOMETRYMANAGER_H
#define GEOMETRYMANAGER_H

#include "D3D11App.h"
#include "DeviceResources.h"

using Microsoft::WRL::ComPtr;
namespace Graphics {
class GeometryManager final {
public:
    // The Big Three
    GeometryManager();
    GeometryManager(const GeometryManager& other);
    GeometryManager& operator=(const GeometryManager& other);

    void Initialize(DeviceResources* a_DR);

    // Accessors for member fields
    ID3D11Buffer*       GetVertexBuffer() const { return m_pVertexBuffer.Get(); }
    ID3D11Buffer*       GetIndexBuffer()  const { return m_pIndexBuffer.Get();  }
    ID3D11VertexShader* GetVertexShader() const { return m_pVertexShader.Get(); }
    ID3D11PixelShader*  GetPixelShader()  const { return m_pPixelShader.Get();  }
    UINT                GetBufferSize()   const { return m_NumVertices;         }

private:
    void BuildVertexBuffers(DeviceResources* a_DR);
    void BuildIndexBuffers(DeviceResources* a_DR);
    void CompileShaders(DeviceResources* a_DR);

private:
    ComPtr<ID3D11Buffer>       m_pVertexBuffer;
    ComPtr<ID3D11Buffer>       m_pIndexBuffer;
    ComPtr<ID3D11VertexShader> m_pVertexShader;
    ComPtr<ID3D11PixelShader>  m_pPixelShader;
    ComPtr<ID3D11InputLayout>  m_pInputLayout;

    UINT                       m_NumVertices;

};

// Vertex Structure Definition (TEMP)
struct Vertex
{
    float pos[3];
    float color[3];
};

}

#endif
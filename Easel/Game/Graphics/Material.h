/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Material class for shader information
----------------------------------------------*/
#ifndef MATERIAL_H
#define MATERIAL_H

#include "D3D11App.h"
using namespace Microsoft::WRL;
namespace Graphics {
class Material {
public:
    Material(ComPtr<ID3D11VertexShader> a_pVS, ComPtr<ID3D11PixelShader> a_pPS);
    ~Material() = default;

    // Field Getters
    ID3D11VertexShader* GetVertexShader();
    ID3D11PixelShader* GetPixelShader();

private:
    ComPtr<ID3D11PixelShader>  m_pPixelShader;
    ComPtr<ID3D11VertexShader> m_pVertexShader;
};
}

#endif
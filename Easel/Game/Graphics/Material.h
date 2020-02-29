/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Material class for shader information
----------------------------------------------*/
#ifndef MATERIAL_H
#define MATERIAL_H

#include "D3D11App.h"

namespace Graphics {

class Material {
public:
    Material(ID3D11VertexShader* a_pVS, ID3D11PixelShader* a_pPS);
    ~Material() = default;

    // Field Getters
    ID3D11VertexShader* GetVertexShader();
    ID3D11PixelShader* GetPixelShader();

private:
    ID3D11PixelShader*  m_pPixelShader;
    ID3D11VertexShader* m_pVertexShader;
};
}

#endif
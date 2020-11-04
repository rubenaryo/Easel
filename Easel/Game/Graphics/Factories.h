#ifndef FACTORIES_H
#define FACTORIES_H

#include "DXCore.h"
#include "ResourceCodex.h"
#include "Shader.h"

#include <EASTL/hash_map.h>

namespace Rendering {

struct TShaderFactory final
{
    static void CreateVertexShader(const wchar_t* fileName, VertexShader* out_shader, ID3D11Device* device);
    static void CreatePixelShader(const wchar_t* fileName, PixelShader* out_shader, ID3D11Device* device);

private: // For VertexShader
    static void BuildInputLayout(ID3D11ShaderReflection* pReflection, ID3D10Blob* pBlob, VertexShader* out_shader, ID3D11Device* device);
    static void AssignDXGIFormatsAndByteOffsets(D3D11_INPUT_CLASSIFICATION slotClass, D3D11_SIGNATURE_PARAMETER_DESC* paramDescs, UINT numInputs, D3D11_INPUT_ELEMENT_DESC* out_inputParams, uint16_t* out_byteOffsets, uint16_t* out_byteSize);

private: // For PixelShader
};

struct TTextureFactory final
{
    typedef uint32_t TextureID;
    typedef eastl::pair<TextureID, const ResourceBindChord> TexturePair;
    static void LoadAllTextures(ID3D11Device* device, ID3D11DeviceContext* context, eastl::hash_map<TextureID, const ResourceBindChord>& out_texMap);
};

struct MeshFactory final
{
    static MeshID CreateMesh(const char* fileName, const VertexBufferDescription* vertAttr, ID3D11Device* pDevice, Mesh* out_mesh);

private:
    
};

}
#endif
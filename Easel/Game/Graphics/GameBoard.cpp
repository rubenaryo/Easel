#include "GameBoard.h"
#include "ResourceCodex.h"
#include "hash_util.h"

namespace Rendering
{
void GameBoard::Init(ID3D11Device* device)
{
    ResourceCodex const& sg_Codex = ResourceCodex::GetSingleton();

    for (uint32_t i = 0; i != BOARD_WIDTH; ++i)
    {
        for (uint32_t j = 0; j != BOARD_HEIGHT; ++j)
        {
            HexDataCPU[i][j].x = i;
            HexDataCPU[i][j].y = j;
            HexDataCPU[i][j].height = -1.f;
        }
    }

    // Create the dynamic vertex buffer
    D3D11_BUFFER_DESC dynamicDesc = {0};
    dynamicDesc.Usage = D3D11_USAGE_DYNAMIC;
    dynamicDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    dynamicDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    dynamicDesc.MiscFlags = 0;
    dynamicDesc.StructureByteStride = 0;
    dynamicDesc.ByteWidth = sizeof(HexData) * BOARD_WIDTH * BOARD_HEIGHT;

    D3D11_SUBRESOURCE_DATA fixedHexData = {0};
    fixedHexData.pSysMem = &HexDataCPU[0][0];

    COM_EXCEPT(device->CreateBuffer(&dynamicDesc, &fixedHexData, &BoardContext.VertexBuffers[1])); 
    
    const Mesh HexMesh = *sg_Codex.GetMesh(fnv1a("Hexagon"));

    BoardContext.VertexBuffers[0]   = HexMesh.VertexBuffer;
    BoardContext.IndexBuffer        = HexMesh.IndexBuffer;
    BoardContext.BoardMaterial      = sg_Codex.GetMaterial(2);
    BoardContext.Strides[0]         = HexMesh.Stride;
    BoardContext.Strides[1]         = sizeof(HexData);
    BoardContext.IndexCount         = HexMesh.IndexCount;
}

void GameBoard::Render(ID3D11DeviceContext* context)
{
    static const UINT offsets[2] =
    {
        0,
        0
    };

    const VertexShader VS = *BoardContext.BoardMaterial->VS;
    const PixelShader PS = *BoardContext.BoardMaterial->PS;

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetInputLayout(VS.InputLayout);
    context->IASetVertexBuffers(0, 2, &BoardContext.VertexBuffers[0], &BoardContext.Strides[0], &offsets[0]);
    context->IASetIndexBuffer(BoardContext.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set up Materials
    context->VSSetShader(VS.Shader, nullptr, 0);
    context->PSSetShader(PS.Shader, nullptr, 0);
    context->PSSetSamplers(0, 1, &PS.SamplerState);

    context->DrawIndexedInstanced(BoardContext.IndexCount, HEX_COUNT, 0, 0, 0);
}
void GameBoard::Shutdown()
{
    BoardContext.VertexBuffers[1]->Release();
}
}
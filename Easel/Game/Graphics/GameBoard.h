/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/12
Description : Interface for management of the game board
----------------------------------------------*/

#include "COMException.h"
#include "DXCore.h"
#include "Material.h"
#include "Mesh.h"

namespace Rendering {

#define BOARD_WIDTH  8
#define BOARD_HEIGHT 5
#define HEX_COUNT (BOARD_WIDTH * BOARD_HEIGHT)

struct HexData
{
    uint32_t x;
    uint32_t y;
    float height;
};

struct BoardDrawContext
{
    ID3D11Buffer* VertexBuffers[2]; // 0: Hex Vertices, 1: GPU-side Hex data
    ID3D11Buffer* IndexBuffer;
    const Material* BoardMaterial;
    UINT Strides[2];
    UINT IndexCount;
};

struct GameBoard
{
    BoardDrawContext  BoardContext;
    HexData           HexDataCPU[BOARD_WIDTH][BOARD_HEIGHT];    // CPU-side per-hex data

    void Init(ID3D11Device* device);
    void Render(ID3D11DeviceContext* context);
    void Shutdown();
};

}
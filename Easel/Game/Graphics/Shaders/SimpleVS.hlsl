
#include "VS_Common.hlsli"

struct VertexIn
{
    float3 position : POSITION;

    uint2 indices   : INSTANCE_HEX_INDEX;
    float height    : INSTANCE_HEX_HEIGHT;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

static const float PI = 3.14159265f;

VertexOut main(VertexIn vi)
{
    VertexOut vo;

    // Reposition hex relative to array index and adjust the height
    //int odd = (int)(vi.indices.x % 2); // 0 if even, 1 if odd
    float3 hexLocation = vi.position;

    // Not Great Attempt at programmatically doing hexes.
    //uint XCoeff = vi.indices.x;
    //float3 XVector = (odd-1)*float3(-1.75, 0, 0) + odd*2*float3(cos(PI/6), 0, sin(PI/6));
    //uint YCoeff = (uint)odd*((vi.indices.x / 2U)) + vi.indices.y;
    //float3 YVector = float3(0, 0, -1);
    //hexLocation += XCoeff*XVector + YCoeff*2*YVector;
    //hexLocation.y += vi.height;
    //
    //if (odd)
    //    vo.color = float4(1, 0, 0, 1);
    //else
    //    vo.color = float4(1, 1, 1, 1);

    hexLocation += vi.indices.x * float3(2, 0, 0) + vi.indices.y * float3(0, 0, 2);
    hexLocation.y += vi.height;

    vo.position = mul(viewProjection, float4(hexLocation, 1.0f));
    vo.color = float4(0, 0.8, 1, 1);
    return vo;
}
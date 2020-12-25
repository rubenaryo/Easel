
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

    uint odd = (vi.indices.x % 2); // 0 if even, 1 if odd
    float3 hexLocation = vi.position;

    hexLocation += vi.indices.x * float3(2, 0, 0) + vi.indices.y * float3(0, 0, 2) + odd*float3(0,0,1); // Adding vertical offset for odd-numbered columns
    hexLocation.y += vi.height;

    vo.position = mul(viewProjection, float4(hexLocation, 1.0f));
    vo.color = float4(0, 0.8, 1, 1);
    return vo;
}
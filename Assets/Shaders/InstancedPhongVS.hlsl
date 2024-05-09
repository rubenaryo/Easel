struct VertexIn
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
    float3 tangent  : TANGENT;
    float3 binormal : BINORMAL;

    // Instancing
    float4x4 world  : INSTANCE_WORLDMATRIX;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
    float3 worldPos : POSITION;
    float3 tangent  : TANGENT;
    float3 binormal : BINORMAL;
};

// Basic camera matrix passed in every frame
cbuffer VSPerPass : register(b10)
{
    float4x4 vp;
}

VertexOut main( VertexIn vi)
{
    VertexOut vo;

    // Construct wvp matrix
    matrix wvp = mul(vp, vi.world);

    // Transform position by camera matrix
    vo.position = mul(wvp, float4(vi.position, 1.0f));

    // Transform normal too
    vo.normal = mul((float3x3)vi.world, vi.normal);

    // Pass along UVs
    vo.uv = vi.uv;

    // Pass along world position
    vo.worldPos = mul((float3x3)vi.world, vi.position);

    // Transform tangent, binormal
    vo.tangent = mul((float3x3)vi.world, vi.tangent);
    vo.binormal = mul((float3x3)vi.world, vi.binormal);

    vo.color = float4(1, 1, 1, 1);
    
    return vo;
}
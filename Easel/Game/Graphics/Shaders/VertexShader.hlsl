struct VertexIn
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float3 uv       : TEXCOORD;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
    float3 normal   : NORMAL;
};

// Basic camera matrix
cbuffer BasicData : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
}

VertexOut main( VertexIn vi)
{
    VertexOut vo;

    // Construct camera matrix
    matrix wvp = mul(projection, mul(view, world));

    // Transform position by camera matrix
    vo.position = mul(wvp, float4(vi.position, 1.0f));

    // Transform normal too
    vo.normal = mul((float3x3)world, vi.normal);

    vo.color = float4(1, 1, 1, 1);
    
    return vo;
}
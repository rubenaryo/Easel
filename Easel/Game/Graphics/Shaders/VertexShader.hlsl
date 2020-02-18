struct VertexIn
{
    float3 position : POSITION;
    float3 color : COLOR;
};

struct VertexOut
{
    float4 position: SV_POSITION;
    float4 color : COLOR;
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
    vo.color = float4(vi.color,1);
    
    return vo;
}
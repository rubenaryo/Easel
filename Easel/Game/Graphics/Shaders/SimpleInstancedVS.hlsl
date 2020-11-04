struct VertexIn
{
    float3 position : POSITION;

    float4x4 world : INSTANCE_WORLDMATRIX;
};

// Basic camera matrix passed in every frame
cbuffer VSPerPass : register(b10)
{
    float4x4 VP;
}

float4 main( VertexIn vi ) : SV_POSITION
{
    float4x4 wvp = mul(VP, vi.world);

    return mul(wvp, float4(vi.position, 1.0f));
}

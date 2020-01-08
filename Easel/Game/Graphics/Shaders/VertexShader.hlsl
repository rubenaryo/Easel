struct VertexIn
{
    float3 position : POSITION;
    float3 color : COLOR0;
};

struct VertexOut
{
    float4 position: SV_POSITION;
    float4 color : COLOR0;
};

VertexOut main( VertexIn vi)
{
    VertexOut vo;
    vo.position = float4(vi.position, 1.0f);
    vo.color = float4(vi.color,1);
	return vo;
}
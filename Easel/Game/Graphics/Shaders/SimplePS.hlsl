
struct VertexOut
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
};

float4 main(VertexOut vo ) : SV_TARGET
{
	return vo.color; // return green
}
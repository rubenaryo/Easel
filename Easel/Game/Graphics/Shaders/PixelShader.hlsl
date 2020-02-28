struct VertexOut
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
    float3 normal   : NORMAL;
};

float4 main(VertexOut input) : SV_TARGET
{
    input.normal = normalize(input.normal);
    return float4(input.normal, 1);
}
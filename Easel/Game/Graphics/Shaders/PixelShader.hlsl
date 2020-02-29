struct VertexOut
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
    float3 normal   : NORMAL;
};

struct DirectionalLight
{
    float3 diffuse;
    float3 direction;
};

cbuffer LightBuffer : register(b0)
{
    float3 ambientColor;
    DirectionalLight lights[8];
    uint numLights;
}

float4 main(VertexOut input) : SV_TARGET
{
    float3 diffusecolor = lights[1].diffuse;
    return float4(diffusecolor, 1);
    input.normal = normalize(input.normal);
    return float4(input.normal, 1);
}
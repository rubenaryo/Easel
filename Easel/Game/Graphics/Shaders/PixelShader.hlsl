struct VertexOut
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
    float3 worldPos : POSITION;
};

struct DirectionalLight
{
    float3 diffuseColor;
    float3 toLight;
};

cbuffer LightBuffer : register(b0)
{
    float3 ambientColor;
    DirectionalLight directionalLight;
}

float DiffuseAmount(float3 aNormal, float3 aToLight)
{
    // Phong Diffuse
    return saturate(dot(aToLight, aNormal));
}

float4 main(VertexOut input) : SV_TARGET
{
    // Normalize normal vector
    input.normal = normalize(input.normal);

    // Holds the total light for this pixel
    float3 totalLight = 0;

    // Diffuse Color
    totalLight += directionalLight.diffuseColor.rgb * 
        DiffuseAmount(input.normal, directionalLight.toLight);
    
    // Finally, add the ambient color
    totalLight += ambientColor;

    return float4(totalLight, 1);
}
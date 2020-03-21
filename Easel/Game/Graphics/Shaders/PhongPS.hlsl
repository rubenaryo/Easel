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

cbuffer LightBuffer : register(b12)
{
    float3 ambientColor;
    DirectionalLight directionalLight;
    float3 cameraWorldPos;
}

cbuffer MaterialParams : register(b13)
{
    float4 colorTint;
    float  specularity;
}

float DiffuseAmount(float3 aNormal, float3 aToLight)
{
    // Phong Diffuse
    return saturate(dot(aToLight, aNormal));
}

float SpecularPhong(float3 aNormal, float3 aLightDir, float3 aToCamera, float aSpec)
{
    float3 R = reflect(normalize(aLightDir), aNormal);
    return pow(saturate(dot(R, aToCamera)), aSpec);
}

Texture2D diffuseTexture    : register(t0);
Texture2D normalMap         : register(t1);
SamplerState samplerOptions : register(s0);
float4 main(VertexOut input) : SV_TARGET
{
    // Sample diffuse texture
    float3 surfaceColor = diffuseTexture.Sample(samplerOptions, input.uv).rgb;

    // Normalize normal vector
    input.normal = normalize(input.normal);

    // Holds the total light for this pixel
    float3 totalLight = 0;
    float3 toCamera = normalize(cameraWorldPos - input.worldPos);

    // Diffuse Color
    totalLight += directionalLight.diffuseColor.rgb *
        DiffuseAmount(input.normal, directionalLight.toLight);

    // Specular Color
    totalLight += directionalLight.diffuseColor.rgb *
        SpecularPhong(input.normal, -directionalLight.toLight, toCamera, specularity);

    // Finally, add the ambient color
    totalLight += ambientColor;
    
    totalLight *= surfaceColor;

    return float4(totalLight, 1);
}
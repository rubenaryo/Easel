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

float Diffuse(float3 aNormal, float3 aLightDir)
{
    return saturate(dot(aNormal, -aLightDir));
}

float4 main(VertexOut input) : SV_TARGET
{
    input.normal = normalize(input.normal);
    //return float4(lights[0].direction, 1);
    // Holds the total light for this pixel
    float3 totalLight = 0;

    for (unsigned int i = 0; i < 1; ++i)
    {
        float lightAmount = Diffuse(input.normal, lights[i].direction);

        // Colorize the light
        float3 finalLight = lightAmount * lights[i].diffuse;

        // Add to running total
        totalLight += finalLight;
    }

    // Finally, add the ambient color
    totalLight += ambientColor;

    return float4(totalLight, 1);
}
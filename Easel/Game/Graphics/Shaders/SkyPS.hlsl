struct VertexOut
{
    float4 position  : SV_POSITION;
    float3 sampleDir : DIRECTION;
};

TextureCube skyTex : register(t4);
SamplerState samplerOptions : register(s0);

float4 main(VertexOut input) : SV_TARGET
{
    return skyTex.Sample(samplerOptions, input.sampleDir);
}
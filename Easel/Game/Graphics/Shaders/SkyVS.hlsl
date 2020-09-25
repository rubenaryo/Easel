struct VertexIn
{
    float3 position : POSITION;
};

struct VertexOut
{
    float4 position  : SV_POSITION;
    float3 sampleDir : DIRECTION;
};

// Basic camera matrix passed in every frame
cbuffer VSPerPass : register(b10)
{
    matrix view;
    matrix projection;
}

VertexOut main( VertexIn input )
{
    VertexOut output;

    matrix lockedView = view;
    // Remove translation from the view matrix:
    lockedView._14 = 0;
    lockedView._24 = 0;
    lockedView._34 = 0;

    // Transform position by VP matrix
    matrix vp = mul(projection, lockedView);
    output.position = mul(vp, float4(input.position, 1.0f));

    // Prepare for perspective divide = 1
    output.position.z = output.position.w;

    // Sample in the direction of the vertex
    output.sampleDir = input.position;

    return output;
}
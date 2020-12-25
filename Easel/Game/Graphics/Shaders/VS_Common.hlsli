/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/11
Description : Common Functions and Constant Buffers shared across ALL vertex shaders
----------------------------------------------*/
#ifndef VS_COMMON_HLSLI
#define VS_COMMON_HLSLI

// These two vectors rarely get updated and are only used to place the hexes correctly
cbuffer VSHexagonParams : register (b9)
{
    float3 XOffset;
    float3 ZOffset;
}

// Basic camera matrix passed in every frame
cbuffer VSPerPass : register(b10)
{
    float4x4 viewProjection;
}

// Values changed for every entity
cbuffer VSPerStaticEntity : register(b11)
{
    float4x4 world;
}

#endif
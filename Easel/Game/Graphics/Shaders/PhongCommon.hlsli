/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Interface for Quaternion-Based Camera functionality 
----------------------------------------------*/
#ifndef PHONGCOMMON_H
#define PHONGCOMMON_H

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

struct DirectionalLight
{
    float3 diffuseColor;
    float3 toLight;
};



#endif
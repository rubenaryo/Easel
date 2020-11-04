/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/10
Description : Standard includes/ macros for a DirectX 11 app
----------------------------------------------*/
#ifndef DXCORE_H
#define DXCORE_H

// DirectX necessary includes
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11_2.h>
#include <d3d11_3.h>
#include <d3d11_4.h>
#include <d3d11sdklayers.h>
#include <d3d11shader.h>
#include <d3d11shadertracing.h>
#include <d3dcompiler.h>
#include <d3dcommon.h>
#include <d3dcsx.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

// Link Direct3D Libraries
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")


// Debug information
#if defined(DEBUG)
    #include <dxgidebug.h>
#endif

namespace Rendering {

// Array of Possible feature levels
static const D3D_FEATURE_LEVEL s_featureLevels[] =
{
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
    D3D_FEATURE_LEVEL_9_3,
    D3D_FEATURE_LEVEL_9_2,
    D3D_FEATURE_LEVEL_9_1,
};

}

#endif

/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Wrapper for Constant Buffer functionality
----------------------------------------------*/
#ifndef CONSTANTBUFFER_H
#define CONSTANTBUFFER_H

#include "DXCore.h"

namespace Graphics {

// Trivial cbuffer data type
//struct ConstantBuffer
//{
//    ID3D11Buffer* Buffer  =  0;       // The underlying DX-Buffer
//    unsigned int  Slot    = -1;       // The slot to bind to
//    unsigned int  Size    = -1;       // Byte Size
//    //bool          Current = false;    // Can we skip re-binding? @note: This field may be useful if in a future implementation, shaders share cbuffers.
//};

enum class ReservedRegisters : UINT
{
    RR_VS_CAMERA        = 10,
    RR_VS_WORLDMATRIX   = 11,
    RR_VS_UNUSED        = 12,
    RR_VS_UNUSED2       = 13,

    RR_PS_LIGHTS        = 10,
    RR_PS_MATERIAL      = 11,
    RR_PS_UNUSED        = 12,
    RR_PS_UNUSED2       = 13,

    RR_MAX_INDEX        = RR_PS_UNUSED,
};

}

#endif
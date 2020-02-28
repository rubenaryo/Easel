/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Interface definitions for 
IDeviceNotify - Parent of Game, for apps that use deviceresources
DeviceResources - Member of Game, encapsulates the creation and setup of device resources
----------------------------------------------*/
#ifndef DEVICERESOURCES_H
#define DEVICERESOURCES_H
#pragma warning(push)
#pragma warning(disable : 26812) // disable enum class warning, DX's fault

#include "D3D11App.h"

using Microsoft::WRL::ComPtr;
namespace Graphics {

// Interface for any device that uses device resources.
// This is because it needs to be able to handle loss of device so it can recreate the necessary device resources
interface IDeviceNotify
{
    virtual void OnDeviceLost() = 0;
    virtual void OnDeviceRestored() = 0;

protected:
    ~IDeviceNotify() = default;
};

class DeviceResources
{
public:
    // Default Values for Initializer List
    DeviceResources(
        DXGI_FORMAT       backBufferFormat  = DXGI_FORMAT_B8G8R8A8_UNORM,
        DXGI_FORMAT       depthBufferFormat = DXGI_FORMAT_D32_FLOAT,
        UINT              backBufferCount   = 2,
        D3D_FEATURE_LEVEL minFeatureLevel   = D3D_FEATURE_LEVEL_10_0,
        unsigned int      flags             = c_FlipPresent | c_EnableMSAA
    ) noexcept;

private:
    // Option Flags for m_Options field
    static const unsigned int c_FlipPresent  = 1;
    static const unsigned int c_AllowTearing = 1 << 1;
    static const unsigned int c_EnableHDR    = 1 << 2;
    static const unsigned int c_EnableMSAA   = 1 << 3;

public:
    void CreateDeviceResources();
    void CreateWindowSizeDependentResources();
    void SetWindow(HWND, int a_Width, int a_Height);
    bool WindowSizeChanged(int a_Width, int a_Height);
    void HandleDeviceLost();
    void RegisterDeviceNotify(IDeviceNotify* device) { m_pDeviceNotify = device; }
    void UpdateTitleBar(uint32_t a_FPS, uint32_t a_FrameCount);
    void Present();
    void Clear(const FLOAT*);

    // Member field accessors
    ID3D11Device*            GetD3DDevice()         const { return m_pDevice.Get();           }
    ID3D11DeviceContext*     GetD3DDeviceContext()  const { return m_pContext.Get();          }
    IDXGISwapChain*          GetSwapChain()         const { return m_pSwapChain.Get();           }
    D3D_FEATURE_LEVEL        GetDeviceFeatureLevel()const { return m_FeatureLevel;            }
    ID3D11Texture2D*         GetRenderTarget()      const { return m_pRenderTarget.Get();        }
    ID3D11Texture2D*         GetDepthStencil()      const { return m_pDepthStencil.Get();        }
    ID3D11RenderTargetView*  GetRenderTargetView()  const { return m_pRenderTargetView.Get(); }
    ID3D11DepthStencilView*  GetDepthStencilView()  const { return m_pDepthStencilView.Get(); }
    DXGI_FORMAT              GetBackBufferFormat()  const { return m_BackBufferFormat;           }
    DXGI_FORMAT              GetDepthBufferFormat() const { return m_DepthBufferFormat;          }
    D3D11_VIEWPORT           GetScreenViewport()    const { return m_ScreenViewport;             }
    UINT                     GetBackBufferCount()   const { return m_BackBufferCount;            }
    DXGI_COLOR_SPACE_TYPE    GetColorSpace()        const { return m_ColorSpaceType;             }
    unsigned int             GetDeviceOptions()     const { return m_Options;                    }
    RECT                     GetOutputSize()        const { return m_OutputSize;                 }
    HWND                     GetWindow()            const { return m_Window;                     }

private:
    void CreateFactory();
    void GetHardwareAdapter(IDXGIAdapter1** ppAdapter);
    void UpdateColorSpace();

    // Direct3D Objects
    // I use ComPtr here to utilize the underlying refcount functionality
    ComPtr<IDXGIFactory2>               m_pDXGIFactory;
    ComPtr<ID3D11Device>                m_pDevice;
    ComPtr<ID3D11DeviceContext>         m_pContext;
    ComPtr<IDXGISwapChain1>             m_pSwapChain;
    ComPtr<ID3DUserDefinedAnnotation>   m_pAnnotation;
                                        
    // Direct3D Rendering Objects       
    ComPtr<ID3D11Texture2D>             m_pRenderTarget;
    ComPtr<ID3D11Texture2D>             m_pDepthStencil;
    ComPtr<ID3D11RenderTargetView>      m_pRenderTargetView;
    ComPtr<ID3D11DepthStencilView>      m_pDepthStencilView;
    D3D11_VIEWPORT                      m_ScreenViewport;
                                        
    // Necessary members for MSAA       
    ComPtr<ID3D11Texture2D>             m_pMsaaRenderTarget;
    ComPtr<ID3D11RenderTargetView>      m_pMsaaRenderTargetView;
    ComPtr<ID3D11DepthStencilView>      m_pMsaaDepthStencilView;
    unsigned int                        m_MsaaSampleCount;
                                        
    // Direct3D Properties              
    DXGI_FORMAT                         m_BackBufferFormat;
    DXGI_FORMAT                         m_DepthBufferFormat;
    UINT                                m_BackBufferCount;
    D3D_FEATURE_LEVEL                   m_MinFeatureLevel;
                                        
                                        
    // Cached Device Properties         
    HWND                                m_Window;
    D3D_FEATURE_LEVEL                   m_FeatureLevel;
    RECT                                m_OutputSize;
                                        
    // HDR Support                      
    DXGI_COLOR_SPACE_TYPE               m_ColorSpaceType;
                                        
    // DeviceResource Options           
    unsigned int                        m_Options;
                                        
    // Pointer to DeviceNotify Interface
    IDeviceNotify*                      m_pDeviceNotify;

};

}
#endif
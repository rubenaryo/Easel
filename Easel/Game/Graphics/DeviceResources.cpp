/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Implementation of DeviceResources.h
- Handles the creation/initialization of DeviceResources
- Modification of resources when needed
- Handles necessary behaviour on loss of device
----------------------------------------------*/
#include "DeviceResources.h"

namespace Graphics {

// Initializer List for Member Fields
DeviceResources::DeviceResources(
    DXGI_FORMAT       a_BackBufferFormat,
    DXGI_FORMAT       a_DepthBufferFormat,
    UINT              a_BackBufferCount,
    D3D_FEATURE_LEVEL a_MinFeatureLevel,
    unsigned int      a_Flags) noexcept :

    m_ScreenViewport    ({}),
    m_BackBufferFormat  (a_BackBufferFormat),
    m_DepthBufferFormat (a_DepthBufferFormat),
    m_BackBufferCount   (a_BackBufferCount),
    m_D3DMinFeatureLevel(a_MinFeatureLevel),
    m_Window            (nullptr),
    m_D3DFeatureLevel   (D3D_FEATURE_LEVEL_9_1),
    m_OutputSize        ({0,0,1,1}),
    m_ColorSpaceType    (DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709),
    m_Options           (a_Flags | c_FlipPresent),
    m_pDeviceNotify     (nullptr)
{}

void DeviceResources::CreateDeviceResources()
{
    UINT createFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

    #if defined(_DEBUG)
    createFlags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

    CreateFactory();

    // TOOD: Check for Tearing Support
    if (m_Options & c_AllowTearing)
    {}

    // TODO: Disable HDR if we are on an OS that can't support FLIP swap effects

    // Disable FLIP if the OS doesn't support it.
    if (m_Options & c_FlipPresent)
    {
        ComPtr<IDXGIFactory4> factory4;
        if (FAILED(m_pDXGIFactory.As(&factory4)))
        {
            m_Options &= ~c_FlipPresent;

            #ifdef _DEBUG
            OutputDebugStringA("INFO: Flip swap effects not supported");
            #endif
        }
    }

    // Determine the feature levels to support
    UINT featLevelCount = 0;
    while(featLevelCount < _countof(s_featureLevels))
    {
        if (s_featureLevels[featLevelCount] < m_D3DMinFeatureLevel)
            break;

        featLevelCount++;
    }

    if (!featLevelCount) throw std::out_of_range("Minimum feature Level too high");

    // POSSIBLE TODO: Get a hardware adapter

    // Create the Direct3D 11 API device object and a corresponding context.
    ComPtr<ID3D11Device>        device;
    ComPtr<ID3D11DeviceContext> context;

    HRESULT hr = E_FAIL;

    hr = D3D11CreateDevice(
        0,                        // pAdapter (IDXGIAdapter*): Using default adapter 
        D3D_DRIVER_TYPE_HARDWARE, // DriverType (D3D_DRIVER_TYPE): Always hardware for now
        0,                        // Software Driver (HMODULE): N/A
        createFlags,              // Device Creation Flags (UINT) : Debug flag enabled for debug build, 0 otherwise
        s_featureLevels,          // Feature Level Array
        featLevelCount,           // Feature Level Count
        D3D11_SDK_VERSION,        // SDK Version (UINT) : Always D3D11_SDK_VERSION
        device.GetAddressOf(),    // ID3D11Device** : Pass device pointer to member field
        &m_D3DFeatureLevel,       // D3D_FEATURE_LEVEL* : Return first supported feature level
        context.GetAddressOf()    // ID3D11DeviceContext** : Pass created device context to member field
    );

    if (FAILED(hr))
    {
        // If the initialization fails, fall back to the WARP device.
        hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_WARP, // Create a WARP device instead of a hardware device.
            nullptr,
            createFlags,
            s_featureLevels,
            featLevelCount,
            D3D11_SDK_VERSION,
            device.GetAddressOf(),
            &m_D3DFeatureLevel,
            context.GetAddressOf()
        );

        if(SUCCEEDED(hr))
        OutputDebugStringA("INFO: Falling back to using WARP\n");
    }

    // Check for failure when creating device
    ThrowIfFailed(hr);

    // Hold created device and context as class members
    ThrowIfFailed(device.As(&m_pD3DDevice));
    ThrowIfFailed(context.As(&m_pD3DContext));
    ThrowIfFailed(context.As(&m_pD3DAnnotation));
}

// Wrapper function for CreateDXGIFactory
// Retrieves the REFIID and checks the HRESULT of the function for failure
void DeviceResources::CreateFactory()
{
    ThrowIfFailed(CreateDXGIFactory(IID_PPV_ARGS(m_pDXGIFactory.ReleaseAndGetAddressOf())));
}

void DeviceResources::GetHardwareAdapter(IDXGIAdapter1** ppAdapter)
{

}

void DeviceResources::UpdateColorSpace()
{
    
}

// Callback method for when we initialize the window, or when there's a need to recreate the resources that are dependent on window size (when the window is resized basically)
void DeviceResources::CreateWindowSizeDependentResources()
{
    if (!m_Window)
        throw std::exception("m_Window member not set!");

    // Clear current window dependent fields.
    m_pD3DContext->OMSetRenderTargets(0u, NULL, NULL);  // Reset bound render targets
    m_pD3DRenderTargetView.Reset();
    m_pD3DDepthStencilView.Reset();
    m_pD3DDepthStencilView.Reset();
    m_pRenderTarget.Reset();
    m_pDepthStencil.Reset();
    m_pD3DContext->Flush();

    // Find the height of the render target, using std::max to ensure neither dimension is 0
    UINT bbWidth = std::max<UINT>(static_cast<UINT>((m_OutputSize.right - m_OutputSize.left)), 1u);
    UINT bbHeight = std::max<UINT>(static_cast<UINT>((m_OutputSize.bottom - m_OutputSize.top)), 1u);
    DXGI_FORMAT bbFormat = m_BackBufferFormat;

    if (m_pSwapChain)
    {
        // Resize an already existing swap chain
        HRESULT hr;
        hr = m_pSwapChain->ResizeBuffers(
            m_BackBufferCount,
            bbWidth,
            bbHeight,
            bbFormat,
            (m_Options & c_AllowTearing) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u
        );

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            char buf[256];
            sprintf_s(buf, "Device Lost on Resize Buffers: Reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ? m_pD3DDevice->GetDeviceRemovedReason() : hr);
            OutputDebugStringA(buf);

            // A new device and swapchain need to be created
            HandleDeviceLost();

            return;
        }
        else
        {
            ThrowIfFailed(hr);
        }
    }
    else // Create a new swap chain
    {
        // Fill out the necessary descriptor structures
        DXGI_SWAP_CHAIN_DESC1 scDesc = {0};
        scDesc.Width = bbWidth;
        scDesc.Height = bbHeight;
        scDesc.Format = bbFormat;
        scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scDesc.BufferCount = m_BackBufferCount;

        //TODO: Evaluate the need to support 4xMSAA
        scDesc.SampleDesc.Count = 1;
        scDesc.SampleDesc.Quality = 0;
        scDesc.Scaling = DXGI_SCALING_STRETCH;
        scDesc.SwapEffect = (m_Options & (c_FlipPresent | c_AllowTearing | c_EnableHDR)) ? DXGI_SWAP_EFFECT_FLIP_DISCARD : DXGI_SWAP_EFFECT_DISCARD;
        scDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        scDesc.Flags = (m_Options & c_AllowTearing) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc = {0};
        fsDesc.Windowed = TRUE;

        ThrowIfFailed(m_pDXGIFactory->CreateSwapChainForHwnd(
            m_pD3DDevice.Get(),
            m_Window,
            &scDesc,
            &fsDesc,
            NULL,
            m_pSwapChain.ReleaseAndGetAddressOf()
        ));

        // Prevent Alt+Enter by monitoring the app message queue
        ThrowIfFailed(m_pDXGIFactory->MakeWindowAssociation(m_Window, DXGI_MWA_NO_ALT_ENTER));
    
        // Create a Render Target View of the SC Back Buffer
        ThrowIfFailed(m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(m_pRenderTarget.ReleaseAndGetAddressOf())));

        // Create a new Render Target View Description
        CD3D11_RENDER_TARGET_VIEW_DESC RTVD(D3D11_RTV_DIMENSION_TEXTURE2D, m_BackBufferFormat);

        // Create a Render Target View from this Description, holding it as a member variable
        ThrowIfFailed(m_pD3DDevice->CreateRenderTargetView(
            m_pRenderTarget.Get(),
            &RTVD,
            m_pD3DRenderTargetView.ReleaseAndGetAddressOf()
        ));

        if (m_DepthBufferFormat != DXGI_FORMAT_UNKNOWN)
        {
            // Fill out descriptor to create Depth Stencil Buffer, holding onto it as a member
            CD3D11_TEXTURE2D_DESC depthStencilDesc(
                m_DepthBufferFormat,
                bbWidth,
                bbHeight,
                1, // This depth stencil view has only one texture.
                1, // Use a single mipmap level.
                D3D11_BIND_DEPTH_STENCIL
            );
            ThrowIfFailed(m_pD3DDevice->CreateTexture2D(
                &depthStencilDesc,
                nullptr,
                m_pDepthStencil.ReleaseAndGetAddressOf()
            ));

            // Fill out descriptor to create Depth Stencil View, holding onto it as a member
            CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
            ThrowIfFailed(m_pD3DDevice->CreateDepthStencilView(
                m_pDepthStencil.Get(),
                &depthStencilViewDesc,
                m_pD3DDepthStencilView.ReleaseAndGetAddressOf()
            ));
        }

        // Set the 3D rendering viewport to target the entire window.
        m_ScreenViewport = CD3D11_VIEWPORT(
            0.0f,
            0.0f,
            static_cast<float>(bbWidth),
            static_cast<float>(bbHeight)
        );
    }
}

// Setter for member fields that may change depending on window state
void DeviceResources::SetWindow(HWND a_Window, int a_Width, int a_Height)
{
    m_Window = a_Window;
    m_OutputSize.left = 0;
    m_OutputSize.top = 0;
    m_OutputSize.right = a_Width;
    m_OutputSize.bottom = a_Height;
}

// Handle Window Size being changed
bool DeviceResources::WindowSizeChanged(int a_Width, int a_Height)
{
    RECT newRc;
    newRc.left = newRc.top = 0;
    newRc.right = a_Width;
    newRc.bottom = a_Height;
    if (newRc == m_OutputSize)
    {
        // Handle color space settings for HDR
        UpdateColorSpace();

        return false;
    }

    m_OutputSize = newRc;
    CreateWindowSizeDependentResources();
    return true;
}

// Re-creates all device resources
void DeviceResources::HandleDeviceLost()
{
    if (m_pDeviceNotify)
        m_pDeviceNotify->OnDeviceLost();

    m_pD3DDepthStencilView.Reset();
    m_pD3DRenderTargetView.Reset();
    m_pRenderTarget.Reset();
    m_pDepthStencil.Reset();
    m_pSwapChain.Reset();
    m_pD3DContext.Reset();
    m_pD3DAnnotation.Reset();

    #ifdef _DEBUG
    {
        ComPtr<ID3D11Debug> d3dDebug;
        if (SUCCEEDED(m_pD3DDevice.As(&d3dDebug)))
        {
            d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY);
        }
    }
    #endif

    m_pD3DDevice.Reset();
    m_pDXGIFactory.Reset();

    // Recreate all device resources
    CreateDeviceResources();
    CreateWindowSizeDependentResources();

    if (m_pDeviceNotify)
        m_pDeviceNotify->OnDeviceRestored();

}

// Presents the contents of the swap chain
void DeviceResources::Present()
{
    HRESULT hr;
    if (m_Options & c_AllowTearing)
    {
        // Recommended to always use tearing if supported when using a sync interval of 0.
        hr = m_pSwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
    }
    else
    {
        // The first argument instructs DXGI to block until VSync, putting the application
        // to sleep until the next VSync. This ensures we don't waste any cycles rendering
        // frames that will never be displayed to the screen.
        hr = m_pSwapChain->Present(1, 0);
    }

    // Clear the Render Target View to Blue
    m_pD3DContext->ClearRenderTargetView(m_pD3DRenderTargetView.Get(), reinterpret_cast<const float*>(&DirectX::Colors::CornflowerBlue));

    // Clear the depth buffer to 1.0f and the stencil buffer to 0.
    m_pD3DContext->ClearDepthStencilView(m_pD3DDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0U);

    // If the device was removed either by a disconnection or a driver upgrade, we
    // must recreate all device resources.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        #ifdef _DEBUG
        char buff[64] = {0};
        sprintf_s(buff, "Device Lost on Present: Reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ? m_pD3DDevice->GetDeviceRemovedReason() : hr);
        OutputDebugStringA(buff);
        #endif

        HandleDeviceLost();
    }
    else
    {
        ThrowIfFailed(hr);

        if (!m_pDXGIFactory->IsCurrent())
        {
            // Output information is cached on the DXGI Factory. If it is stale we need to create a new factory.
            CreateFactory();
        }
    }
}

}
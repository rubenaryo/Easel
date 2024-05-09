/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Implementation of DeviceResources.h
- Handles the creation/initialization of DeviceResources
- Modification of resources when needed
- Handles necessary behaviour on loss of device
----------------------------------------------*/
#include "DeviceResources.h"

#include "ThrowMacros.h"
#include <algorithm>

#if defined(ESL_DEBUG)
#include <sstream>   // wstringstream
#include <stdexcept> // std::exception
#endif

namespace Renderer {

// Initializer List for Member Fields
DeviceResources::DeviceResources(
    DXGI_FORMAT       backBufferFormat,
    DXGI_FORMAT       depthBufferFormat,
    UINT              backBufferCount,
    D3D_FEATURE_LEVEL minFeatureLevel,
    uint8_t           options) noexcept
    :   mViewportInfo      ({})
    ,   mBackBufferFormat  (backBufferFormat)
    ,   mDepthBufferFormat (depthBufferFormat)
    ,   mBackBufferCount   (backBufferCount)
    ,   mMinFeatureLevel   (minFeatureLevel)
    ,   mpDXGIFactory      (nullptr)
    ,   mPresentFlags      (0)
    ,   mWindow            (nullptr)
    ,   mFeatureLevel      (D3D_FEATURE_LEVEL_9_1)
    ,   mOutputSize        ({0,0,1,1})
    ,   mColorSpaceType    (DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709)
    ,   mDeviceOptions     (options)
    ,   mpRenderTargetView (nullptr)
    ,   mpDepthStencilView (nullptr)
    ,   mpMSAARenderTargetView(nullptr)
    ,   mpMSAADepthStencilView(nullptr)
    ,   mpMSAARenderTarget(nullptr)
    ,   mpRenderTarget(nullptr)
    ,   mpDepthStencilTex(nullptr)
    ,   mMSAASampleCount   (4)
    ,   mpDeviceNotify     (nullptr)
    ,   mpSwapChain     (nullptr)
    #if defined(ESL_DEBUG)
    ,   mpDebugInterface   (nullptr)
    #endif
{}

// This initializes the following fields:
// - Device
// - Context
void DeviceResources::CreateDeviceResources()
{
    UINT createFlags = 0;

    #if defined(ESL_DEBUG)
    createFlags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

    CreateFactory();

    // TODO: Disable HDR if we are on an OS that can't support FLIP swap effects

    HRESULT hr;
    // Disable FLIP if the OS doesn't support it. @note: This should probably be removed later, as non-flip is deprecated
    if (OptionEnabled(DR_OPTIONS::DR_FLIP_PRESENT))
    {
        IDXGIFactory4* factory4;
        hr = mpDXGIFactory->QueryInterface(__uuidof(IDXGIFactory4), reinterpret_cast<void**>(&factory4));
        if (FAILED(hr))
        {
            mDeviceOptions &= ~(uint8_t)DR_OPTIONS::DR_FLIP_PRESENT;

            #if defined(ESL_DEBUG)
            OutputDebugStringA("WARNING: Flip swap effects not supported\n");
            #endif
        }
    }

    // Determine the feature levels to support
    UINT featLevelCount = 0;
    while(featLevelCount < _countof(s_featureLevels))
    {
        if (s_featureLevels[featLevelCount] < mMinFeatureLevel)
            break;

        featLevelCount++;
    }

    #if defined(ESL_DEBUG)
    if (!featLevelCount) throw std::out_of_range("Minimum feature level too high");
    #endif
    // POSSIBLE TODO: Get a hardware adapter

    hr = D3D11CreateDevice(
        0,                        // pAdapter (IDXGIAdapter*): Using default adapter 
        D3D_DRIVER_TYPE_HARDWARE, // DriverType (D3D_DRIVER_TYPE): Always hardware for now
        0,                        // Software Driver (HMODULE): N/A
        createFlags,              // Device Creation Flags (UINT) : Debug flag enabled for debug build, 0 otherwise
        s_featureLevels,          // Feature Level Array
        featLevelCount,           // Feature Level Count
        D3D11_SDK_VERSION,        // SDK Version (UINT) : Always D3D11_SDK_VERSION
        &mpDevice,                // ID3D11Device** : Pass device pointer to member field
        &mFeatureLevel,           // D3D_FEATURE_LEVEL* : Return first supported feature level
        &mpContext                // ID3D11DeviceContext** : Pass created device context to member field
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
            &mpDevice,
            &mFeatureLevel,
            &mpContext
        );
        
        #if defined(ESL_DEBUG)
        OutputDebugStringA("INFO: Falling back to using WARP\n");
        #endif
    }

    // Check for failure when creating device
    COM_EXCEPT(hr);

    // Check for MSAA Support
    while (mMSAASampleCount > 1)
    {
        UINT levels = 0;
        if (FAILED(mpDevice->CheckMultisampleQualityLevels(mBackBufferFormat, mMSAASampleCount, &levels)))
            continue;

        if (levels > 0)
            break;

        // Decrease Sample count by powers of two
        mMSAASampleCount /= 2;
    }

    // MSAA not supported or not a power of two or too high
    if (mMSAASampleCount < 2 || mMSAASampleCount % 2 || mMSAASampleCount > 16)
    {
        // Turn off MSAA support
        mDeviceOptions &= ~(uint8_t)DR_OPTIONS::DR_ENABLE_MSAA;

        // Ensure Consistency of sample count
        mMSAASampleCount = 1;
    }
    else
    {
        // Ensure MSAA flag is on
        mDeviceOptions |= (uint8_t)DR_OPTIONS::DR_ENABLE_MSAA;
    }
    
#if defined(ESL_DEBUG) // Create annotation and debug interface
    // Populate User Defined Annotation Member
    hr = mpContext->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), reinterpret_cast<void**>(&mpAnnotation));
    COM_EXCEPT(hr); //@note: Might be ok to let this fail, and just disable its behavior.

    // Populate Debug Interface Member
    hr = mpDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&mpDebugInterface));

    // Query the D3D Info Queue Interface:
    if (SUCCEEDED(hr))
    {
        ID3D11InfoQueue* d3dInfoQueue = nullptr;
        hr = mpDebugInterface->QueryInterface(__uuidof(ID3D11InfoQueue), reinterpret_cast<void**>(&d3dInfoQueue));
        if (SUCCEEDED(hr))
        {
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
            D3D11_MESSAGE_ID hide[] =
            {
                D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
            };
            D3D11_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = _countof(hide);
            filter.DenyList.pIDList = hide;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
            d3dInfoQueue->Release();
        }
    }

    const char deviceName[] = "DR_Device";
    hr = mpDevice->SetPrivateData(WKPDID_D3DDebugObjectName, ARRAYSIZE(deviceName) - 1, deviceName);

    COM_EXCEPT(hr);


#endif
}

// Wrapper function for CreateDXGIFactory
// Retrieves the REFIID and checks the HRESULT of the function for failure
void DeviceResources::CreateFactory()
{
    if (mpDXGIFactory) mpDXGIFactory->Release();
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&mpDXGIFactory));
    COM_EXCEPT(hr);
}

// Callback method for when we initialize the window, or when there's a need to recreate the resources that are dependent on window size (when the window is resized basically)
// This initializes the following members:
// - Context
// - Render Target View
// - MSAA Render Target View
// - Depth/Stencil View
// - MSAA Depth/Stencil View
// - Render Target
// - MSAA Render Target
// - Depth Stencil Buffer
void DeviceResources::CreateWindowSizeDependentResources()
{
    #if defined(ESL_DEBUG)
    if (!mWindow)
        throw std::exception("mWindow member not set!");
    #endif

    // Clear current window dependent fields.
    mpContext->OMSetRenderTargets(0, NULL, NULL);  // Reset bound render targets

    #pragma region Cleanup COM Objects
    if (mpRenderTargetView)
    {
        mpRenderTargetView->Release();
        mpRenderTargetView = nullptr;
    }
    if (mpDepthStencilView)
    {
        mpDepthStencilView->Release();
        mpDepthStencilView = nullptr;
    }
    if (mpMSAARenderTargetView)
    {
        mpMSAARenderTargetView->Release();
        mpMSAARenderTargetView = nullptr;
    }
    if (mpMSAADepthStencilView)
    {
        mpMSAADepthStencilView->Release();
        mpMSAARenderTargetView = nullptr;
    }
    if (mpMSAARenderTarget)
    {
        mpMSAARenderTarget->Release();
        mpMSAARenderTarget = nullptr;
    }
    if (mpRenderTarget)
    {
        mpRenderTarget->Release();
        mpRenderTarget = nullptr;
    }
    if (mpDepthStencilTex)
    {
        mpDepthStencilTex->Release();
        mpDepthStencilTex = nullptr;
    }
    mpContext->Flush();
    #pragma endregion

    // Find the height of the render target, using std::max to ensure neither dimension is 0
    UINT backBufferWidth  = std::max<UINT>(static_cast<UINT>((mOutputSize.right - mOutputSize.left)), 1U);
    UINT backBufferHeight = std::max<UINT>(static_cast<UINT>((mOutputSize.bottom - mOutputSize.top)), 1U);
    DXGI_FORMAT backBufferFormat = mBackBufferFormat;

    HRESULT hr;
    if (mpSwapChain)
    {
        bool vsyncOff = OptionEnabled(DR_OPTIONS::DR_ALLOW_TEARING);

        // Resize an existing swap chain
        hr = mpSwapChain->ResizeBuffers(
            mBackBufferCount,
            backBufferWidth,
            backBufferHeight,
            backBufferFormat,
            OptionEnabled(DR_OPTIONS::DR_ALLOW_TEARING) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0
        );

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            #if defined(ESL_DEBUG)
            char buf[256];
            sprintf_s(buf, "Device Lost on Resize Buffers: Reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ? mpDevice->GetDeviceRemovedReason() : hr);
            OutputDebugStringA(buf);
            #endif

            // A new device and swapchain need to be created
            HandleDeviceLost();

            return;
        }

        COM_EXCEPT(hr);
    }
    else // Create a new swap chain
    {
        // Swap Chain Creation:
        // - Hardcoded 60Hz
        // - Blt (non-flip) effects conditionally supported. In the future, it may be better to outright disable this deprecated behavior.
        DXGI_SWAP_CHAIN_DESC swapDesc = {};
        swapDesc.BufferCount = 2;
        swapDesc.BufferDesc.Width = backBufferWidth;
        swapDesc.BufferDesc.Height = backBufferHeight;
        swapDesc.BufferDesc.RefreshRate.Numerator = 60;
        swapDesc.BufferDesc.RefreshRate.Denominator = 1;
        swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapDesc.OutputWindow = mWindow;
        swapDesc.SwapEffect = OptionEnabled(DR_OPTIONS::DR_FLIP_PRESENT) ? DXGI_SWAP_EFFECT_FLIP_DISCARD : DXGI_SWAP_EFFECT_DISCARD;
        swapDesc.SampleDesc.Count = 1;
        swapDesc.SampleDesc.Quality = 0;
        swapDesc.Windowed = true;

        // VSYNC check
        if (OptionEnabled(DR_OPTIONS::DR_ALLOW_TEARING))
        {
            swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
            mPresentFlags = DXGI_PRESENT_ALLOW_TEARING;
        }
        else
        {
            swapDesc.Flags = 0;
            mPresentFlags = 0;
        }

        hr = mpDXGIFactory->CreateSwapChain(
            mpDevice,
            &swapDesc,
            &mpSwapChain
        );
        COM_EXCEPT(hr);

        #pragma region Prevent Alt+Enter by monitoring the app message queue 
        // Get correct IDXGIFactory
        IDXGIDevice* pDXGIDevice;
        hr = mpDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);
        COM_EXCEPT(hr);
        IDXGIAdapter* pDXGIAdapter;
        hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pDXGIAdapter);
        COM_EXCEPT(hr);
        IDXGIFactory* pIDXGIFactory;
        hr = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pIDXGIFactory);
        COM_EXCEPT(hr);

        // Make the window association
        hr = pIDXGIFactory->MakeWindowAssociation(mWindow, DXGI_MWA_NO_WINDOW_CHANGES);
        COM_EXCEPT(hr);

        pIDXGIFactory->Release();
        pDXGIAdapter->Release();
        pDXGIDevice->Release();
        #pragma endregion

        // Create a Render Target of the SC Back Buffer
        if (mpRenderTarget) mpRenderTarget->Release();
        hr = mpSwapChain->GetBuffer(0, IID_PPV_ARGS(&mpRenderTarget));
        COM_EXCEPT(hr);

        // Create a new Render Target View Description
        CD3D11_RENDER_TARGET_VIEW_DESC RTVD(D3D11_RTV_DIMENSION_TEXTURE2D, mBackBufferFormat);

        // Create a Render Target View from this Description, holding it as a member variable
        if (mpRenderTargetView) mpRenderTargetView->Release();
        hr = mpDevice->CreateRenderTargetView(
            mpRenderTarget,
            0,
            &mpRenderTargetView
        );
        COM_EXCEPT(hr);

        if (mDepthBufferFormat != DXGI_FORMAT_UNKNOWN)
        {
            // Fill out descriptor to create Depth Stencil Buffer, holding onto it as a member
            CD3D11_TEXTURE2D_DESC depthStencilDesc(
                mDepthBufferFormat,
                backBufferWidth,
                backBufferHeight,
                1, // This depth stencil view has only one texture.
                1, // Use a single mipmap level.
                D3D11_BIND_DEPTH_STENCIL
            );

            if (mpDepthStencilTex) mpDepthStencilTex->Release();
            hr = mpDevice->CreateTexture2D(
                &depthStencilDesc,
                nullptr,
                &mpDepthStencilTex);
            COM_EXCEPT(hr);

            // Fill out descriptor to create Depth Stencil View, holding onto it as a member
            if (mpDepthStencilView) mpDepthStencilView->Release();
            CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
            hr = mpDevice->CreateDepthStencilView(
                mpDepthStencilTex,
                &depthStencilViewDesc,
                &mpDepthStencilView
            );
            COM_EXCEPT(hr);

            #pragma region MSAA Stuff
            if (OptionEnabled(DR_OPTIONS::DR_ENABLE_MSAA))
            {
                // Create an MSAA Render Target
                CD3D11_TEXTURE2D_DESC MsaaRTD(
                    mBackBufferFormat,
                    backBufferWidth,
                    backBufferHeight,
                    1, // The render target view has only one texture.
                    1, // Use a single mipmap level.
                    D3D11_BIND_RENDER_TARGET,
                    D3D11_USAGE_DEFAULT,
                    0,
                    mMSAASampleCount
                );

                if (mpMSAARenderTarget) mpMSAARenderTarget->Release();
                hr = mpDevice->CreateTexture2D(
                    &MsaaRTD,
                    nullptr,
                    &mpMSAARenderTarget);

                COM_EXCEPT(hr);

                // Create a new MSAA Render Target View Description
                CD3D11_RENDER_TARGET_VIEW_DESC MsaaRTVD(D3D11_RTV_DIMENSION_TEXTURE2DMS, mBackBufferFormat);

                // Create an MSAA Render Target View from this Description, holding it as a member variable
                if (mpMSAARenderTargetView) mpMSAARenderTargetView->Release();
                hr = mpDevice->CreateRenderTargetView(
                    mpMSAARenderTarget,
                    &MsaaRTVD,
                    &mpMSAARenderTargetView);
                COM_EXCEPT(hr);

                CD3D11_TEXTURE2D_DESC MsaaDSD(
                    mDepthBufferFormat,
                    backBufferWidth,
                    backBufferHeight,
                    1, // This depth stencil view has only one texture.
                    1, // Use a single mipmap level.
                    D3D11_BIND_DEPTH_STENCIL,
                    D3D11_USAGE_DEFAULT,
                    0,
                    mMSAASampleCount
                );

                // fill temporary texture
                ID3D11Texture2D* MsaaDepthStencil = nullptr;
                hr = mpDevice->CreateTexture2D(
                    &MsaaDSD,
                    nullptr,
                    &MsaaDepthStencil
                );
                COM_EXCEPT(hr);

                // fill member MsaaDepthStencilView
                if (mpMSAADepthStencilView) mpMSAADepthStencilView->Release();
                hr = mpDevice->CreateDepthStencilView(
                    MsaaDepthStencil,
                    nullptr,
                    &mpMSAADepthStencilView
                );
                COM_EXCEPT(hr);
                MsaaDepthStencil->Release();
            }
            #pragma endregion
        }

        // Set the 3D rendering viewport to target the entire window.
        #if defined(ESL_DEBUG)
        assert(sizeof(FLOAT) == sizeof(backBufferWidth));
        assert(sizeof(FLOAT) == sizeof(backBufferHeight));
        #endif

        mViewportInfo = CD3D11_VIEWPORT(
            0.0f,                                   // TopLeft X Coord
            0.0f,                                   // TopLeft Y Coord
            static_cast<FLOAT>(backBufferWidth),    // Width
            static_cast<FLOAT>(backBufferHeight),   // Height
            0.0f,                                   // Min Depth
            1.0f                                    // Max Depth
        );
    }

}

// Setter for member fields that may change depending on window state
void DeviceResources::SetWindow(HWND window, int width, int height)
{
    mWindow = window;
    mOutputSize.left = 0;
    mOutputSize.top = 0;
    mOutputSize.right = width;
    mOutputSize.bottom = height;
}

// Handle Window Size being changed
bool DeviceResources::WindowSizeChanged(int width, int height)
{
    RECT newRc;
    newRc.left = newRc.top = 0;
    newRc.right = width;
    newRc.bottom = height;
    if (newRc == mOutputSize)
    {
        // Handle color space settings for HDR
        UpdateColorSpace();

        return false;
    }

    mOutputSize = newRc;
    CreateWindowSizeDependentResources();
    return true;
}

// Presents the contents of the swap chain
void DeviceResources::Present()
{
    HRESULT hr;

    //@todo: LOTS OF BRANCHES HERE! BAD

    // Resolve the MSAA Render Target
    if (OptionEnabled(DR_OPTIONS::DR_ENABLE_MSAA))
    {
        mpContext->ResolveSubresource(mpRenderTarget, 0, mpMSAARenderTarget, 0, mBackBufferFormat);
    }

    // Recommended to always use tearing if supported when using a sync interval of 0.
    hr = mpSwapChain->Present(0, mPresentFlags);

    // If the device was removed either by a disconnection or a driver upgrade, we
    // must recreate all device resources.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        #if defined(ESL_DEBUG)
        static char buf[64];
        ZeroMemory(buf, 64);
        sprintf_s(buf, "Device Lost on Present: Reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ? mpDevice->GetDeviceRemovedReason() : hr);
        OutputDebugStringA(buf);
        #endif

        HandleDeviceLost();
    }
}

// Clears the back buffer to backgroundColor, taking into account MSAA
void DeviceResources::Clear(const FLOAT* backgroundColor)
{
    auto context = mpContext;

    // Clear views @todo: Branch necessary?
    if (OptionEnabled(DR_OPTIONS::DR_ENABLE_MSAA))
    {
        mpContext->ClearRenderTargetView(mpMSAARenderTargetView, backgroundColor);
        mpContext->ClearDepthStencilView(mpMSAADepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        mpContext->OMSetRenderTargets(1, &mpMSAARenderTargetView, mpMSAADepthStencilView);
    }
    else
    {
        mpContext->ClearRenderTargetView(mpRenderTargetView, backgroundColor);
        mpContext->ClearDepthStencilView(mpDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        mpContext->OMSetRenderTargets(1, &mpRenderTargetView, mpDepthStencilView);
    }
    
    // Set the viewport
    mpContext->RSSetViewports(1, &mViewportInfo);
}

#if defined(ESL_DEBUG)
void DeviceResources::ReportLiveDeviceObjects_d()
{
    mpDebugInterface->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
}
#endif

void DeviceResources::ReleaseAllComAndDumpLiveObjects()
{
    if (mpDepthStencilView)
    {
        mpDepthStencilView->Release();
        mpDepthStencilView = nullptr;
    }
    if (mpRenderTargetView)
    {
        mpRenderTargetView->Release();
        mpRenderTargetView = nullptr;
    }
    if (mpRenderTarget)
    {
        mpRenderTarget->Release();
        mpRenderTarget = nullptr;
    }
    if (mpMSAADepthStencilView)
    {
        mpMSAADepthStencilView->Release();
        mpMSAADepthStencilView = nullptr;
    }
    if (mpMSAARenderTarget)
    {
        mpMSAARenderTarget->Release();
        mpMSAARenderTarget = nullptr;
    }
    if (mpMSAARenderTargetView)
    {
        mpMSAARenderTargetView->Release();
        mpMSAARenderTargetView = nullptr;
    }
    if (mpDepthStencilTex)
    {
        mpDepthStencilTex->Release();
        mpDepthStencilTex = nullptr;
    }
    if (mpSwapChain)
    {
        mpSwapChain->Release();
        mpSwapChain = nullptr;
    }
    if (mpContext)
    {
        mpContext->ClearState();
        mpContext->Flush();
        mpContext->Release();
        mpContext = nullptr;
    }
    if (mpDXGIFactory)
    {
        mpDXGIFactory->Release();
        mpDXGIFactory = nullptr;
    }
    if (mpDevice)
    {
        mpDevice->Release();
        mpDevice = nullptr;
    }

#if defined(ESL_DEBUG)
    if (mpAnnotation)
    {
        mpAnnotation->Release();
        mpAnnotation = nullptr;
    }
    if (mpDebugInterface)
    {
        ReportLiveDeviceObjects_d();
        mpDebugInterface->Release();
        mpDebugInterface = nullptr;
    }
#endif
}

DeviceResources::~DeviceResources()
{
    #if defined(ESL_DEBUG)
    OutputDebugStringA("ENGINE: Shutting Down Rendering System...\n");
    #endif
    ReleaseAllComAndDumpLiveObjects();
}

// Re-creates all device resources: Just short of a fatal error
void DeviceResources::HandleDeviceLost()
{
    // Let Game know by invoking callback
    if (mpDeviceNotify)
        mpDeviceNotify->OnDeviceLost();

    // Delete all COM Objects
    ReleaseAllComAndDumpLiveObjects();

    // Recreate ALL device resources
    CreateDeviceResources();
    CreateWindowSizeDependentResources();

    // Try to restore Game
    if (mpDeviceNotify)
        mpDeviceNotify->OnDeviceRestored();
}

void DeviceResources::GetHardwareAdapter(IDXGIAdapter1** ppAdapter)
{
    // TODO: implement later
}

void DeviceResources::UpdateColorSpace()
{
    // TODO: implement later
}

#if defined(ESL_DEBUG)
void DeviceResources::UpdateTitleBar(uint32_t fps, uint32_t frameCount)
{
    // Update title bar every 120 frames
    if (frameCount % 120 == 0)
        return;

    std::wstringstream wss;

    // Window Information
    wss <<  L"Width: "  << mViewportInfo.Width  <<
        L"    Height: " << mViewportInfo.Height <<
        L"    FPS: "    << fps;

    // Check and Print Feature Level
    switch (mFeatureLevel)
    {
    case D3D_FEATURE_LEVEL_11_1: wss << L"    Direct3D 11.1"; break;
    case D3D_FEATURE_LEVEL_11_0: wss << L"    Direct3D 11.0"; break;
    case D3D_FEATURE_LEVEL_10_1: wss << L"    Direct3D 10.1"; break;
    case D3D_FEATURE_LEVEL_10_0: wss << L"    Direct3D 10.0"; break;
    case D3D_FEATURE_LEVEL_9_3:  wss << L"    Direct3D 9.3";  break;
    case D3D_FEATURE_LEVEL_9_2:  wss << L"    Direct3D 9.2";  break;
    case D3D_FEATURE_LEVEL_9_1:  wss << L"    Direct3D 9.1";  break;
    default:                     wss << L"    Direct3D ???";  break;
    }

    // MSAA Level
    wss << L"    " << mMSAASampleCount << L"xMSAA";

    SetWindowText(GetWindow(), wss.str().c_str());
}
#else // This is temporary code for debugging
void DeviceResources::UpdateTitleBar(uint32_t fps, uint32_t frameCount)
{
    // Update title bar every 60 frames
    if (frameCount % 60 == 0)
        return;

    // Small static buffer just for displaying the FPS
    static char buf[16];
    sprintf_s(buf, "FPS: %d", fps);

    SetWindowTextA(GetWindow(), buf);
    
    SecureZeroMemory(buf, 16);
}
#endif

}
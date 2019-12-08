/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/10
Description : Standard includes/ macros for a DirectX 11 app
----------------------------------------------*/
#ifndef D3D11APP_H
#define D3D11APP_H

// Standard Windows Includes
#include "../../System/WinApp.h"
// Exception Interface
#include "../../System/IException.h"

// Link Direct3D Libraries
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// DirectX necessary includes
#include <D3D11.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

// STL Includes
#include <algorithm>
#include <exception>
#include <memory>
#include <stdexcept>
#include <sstream>

// Debug information
#ifdef _DEBUG
#include <dxgidebug.h>
#endif



namespace Graphics {
    
// Helper class for COM Exceptions
class COMException : public IException
{
public:
    COMException(int a_Line, const wchar_t* a_Filename, HRESULT a_HRESULT) noexcept
        : IException(a_Line, a_Filename)
        , m_HRESULT(a_HRESULT)
    {}

    const wchar_t* What() noexcept override
    {
        std::wstringstream oss;
        oss << GetType() << std::endl
            << "[Error Code] " << std::endl << GetErrorCode() << std::endl << std::endl
            << "[Description] " << std::endl << GetErrorString() << std::endl
            << GetOriginString() << std::endl;

        m_buf = oss.str();
        return m_buf.c_str();
    }

    const wchar_t* GetType() const noexcept
    {
        return L"COM Exception";
    }

    std::wstring TranslateErrorCode(HRESULT a_HRESULT) noexcept
    {
        wchar_t* pMsgBuf = NULL;

        const DWORD nMsgLen = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, a_HRESULT, 0,
            reinterpret_cast<LPWSTR>(&pMsgBuf), 0, NULL
        );

        // Failure
        if (nMsgLen == 0)
        {
            return L"Unidentified error code";
        }

        std::wstring errorString = pMsgBuf;

        // Once, copied,we can free the stack buffer
        LocalFree(pMsgBuf);
        return errorString;
    }

    HRESULT GetErrorCode() const noexcept 
    {
        return m_HRESULT;
    }

    std::wstring GetErrorString() noexcept
    {
        std::wstring resized = TranslateErrorCode(m_HRESULT);
        return resized;
    }

private:
    HRESULT m_HRESULT;
};

}

#endif

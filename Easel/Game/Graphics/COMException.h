/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Interface for custom COM Exception reporting
----------------------------------------------*/
#ifndef COMEXCEPTION_H
#define COMEXCEPTION_H

#include <windows.h>
#include <winbase.h>
#include <string>
#include <sstream>
#include <exception>

// Putting this in the graphics namespace, since it's primarily meant to interface with DeviceResources
namespace Graphics {

class COMException : public std::exception
{
public:
    COMException(int a_Line, const char* a_File, HRESULT a_HR) noexcept :
        m_HR(a_HR),
        m_Line(a_Line),
        m_File(a_File),
        errorMsgBuffer(NULL)
    {}

    COMException() = delete;

    ~COMException()
    {
        LocalFree(errorMsgBuffer);
    }

    const char* what() const noexcept override
    {
        std::ostringstream oss;
        oss << GetType() << std::endl
            << "[Error Code:] 0x" << std::hex << std::uppercase << m_HR
            << std::dec << " (" << (unsigned long)m_HR << " )" << std::endl
            << "[Description: ]" << GetErrorMessage() << std::endl
            << GetOriginString();

        whatBuffer = oss.str();
        return whatBuffer.c_str();
    }
    
    const char* GetType() const noexcept
    {
        return "COM Exception";
    }

    HRESULT GetErrorCode() const noexcept
    {
        return m_HR;
    }

    const CHAR* GetErrorMessage() const noexcept
    {
      FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
          FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
          NULL,
          m_HR,
          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
          (LPSTR)&errorMsgBuffer,
          0,
          NULL);

        return errorMsgBuffer;
    }

    int GetLine() const noexcept
    {
        return m_Line;
    }

    const std::string& GetFile() const noexcept
    {
        return m_File;
    }

    std::string GetOriginString() const noexcept
    {
        std::ostringstream oss;
        oss << "[File:] " << m_File << std::endl
            << "[Line:] " << m_Line;
        return oss.str();
    }

private:
    HRESULT     m_HR;
    int         m_Line;
    std::string m_File;

    // must be mutable to be modifiable through const function
    mutable std::string whatBuffer;
    mutable CHAR* errorMsgBuffer;
};

}

#endif
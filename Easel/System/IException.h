/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/10
Description : Interface for custom exception implementation
----------------------------------------------*/
#ifndef IEXCEPTION_H
#define IEXCEPTION_H
#include <exception>
#include <string>

// IException is our custom way of packing error information from windows 
// or the graphics system for display upon hitting a runtime error
// In the future, more custom exception classes will follow (Graphics::Exception, etc)
class IException : public std::exception
{
public: 
    IException(int a_Line, const wchar_t* a_Filename) noexcept;
    virtual const wchar_t* What() const noexcept;
    virtual const wchar_t* GetType() const noexcept = 0;
    const std::wstring& GetFile() const noexcept;
    std::wstring GetOriginString() const noexcept;

private:
    // Store line number and file name locally for when displaying information
    int m_Line;
    std::wstring m_File;

protected:
    // Buffer that holds the info for what16()
    mutable std::wstring m_buf;
};
#endif
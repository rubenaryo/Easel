/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/10
Description : Implementation of custom exception functions
----------------------------------------------*/
#include "IException.h"
#include <sstream>

// Simple constructor to initialize member fields
IException::IException(int a_Line, const wchar_t* a_Filename) noexcept
    : m_Line( a_Line ), m_File(a_Filename) {}

// Collects information about the error and returns the address of our buffer
const wchar_t* IException::What() noexcept
{
    // Write exception information into an output string object
    std::wstringstream oss;
    oss << GetType() << std::endl << GetOriginString();
    
    // Store the value of the string into local buffer
    m_buf = oss.str();

    // Return pointer to c string of the object
    return m_buf.c_str();
}

// Accessor for filename
const std::wstring& IException::GetFile() const noexcept
{
    return m_File;
}

// Gather information about where the exception happened
std::wstring IException::GetOriginString() const noexcept
{
    // Write location info into a std::string object 
    std::wstringstream oss;
    oss << "[File] " << std::endl << m_File << std::endl << std::endl
        << "[Line] " << std::endl << m_Line;
    return oss.str();
}

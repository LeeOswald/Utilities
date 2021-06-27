#pragma once

#include <windows.h>

#include "./IRefCounted.hxx"

#include <string>

namespace Core
{
  
class Error
    : public RefCountedBase
{
public:
    typedef RefCountedPtr<Error> Ref;

    static Ref make(
        const wchar_t* message,
        long code = 0,
        const char* file = nullptr,
        int line = 0,
        Ref inner = nullptr
        ) 
    {
        return Ref(new Error(
            0,
            code,
            message,
            file,
            line,
            inner
            ));
    }

    Error(Error const&) = delete;
    Error& operator=(Error const&) = delete;

    long type() const noexcept
    {
        return m_type;
    }

    const std::wstring& message() const noexcept // this is the original text string passed to make()
    {
        return m_message;
    }

    const char* file() const noexcept
    {
        return m_file;
    }

    int line() const noexcept
    {
        return m_line;
    }

    Ref inner() const noexcept
    {
        return m_inner;
    }

    long code() const noexcept
    {
        return m_code;
    }

    virtual std::wstring errorText() const noexcept // this could be a textual representation of the error code
    {
        return std::wstring();
    }

    // {{ - left curly brace
    // {T} - type
    // {Cd} - error code (decimal)
    // {Cx} - error code (hex)
    // {M} - message that message() returns
    // {S} - text that errorText() returns
    // {MS} - {M} [{S}]
    // {F} - source file name
    // {L} - source line
    // {FL} - source file:line
    virtual std::wstring format(const wchar_t* format) const noexcept;

protected:
    virtual ~Error() noexcept
    {
    }

    Error(
        long type,
        long code,
        const wchar_t* message,
        const char* file,
        int line,
        Ref inner
        ) noexcept
        : m_type(type)
        , m_code(code)
        , m_message(message)
        , m_file(file)
        , m_line(line)
        , m_inner(inner)
    {
    }

private:
    long m_type;
    long m_code;
    std::wstring m_message;
    const char* m_file;
    int m_line;
    Ref m_inner;
};





} // namespace Core {}

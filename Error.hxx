#pragma once

#include <windows.h>

#include "./IRefCounted.hxx"
#include "./Win32.hxx"

#include <string>

namespace Util
{
  
class Error
    : public RefCountedBase<IRefCounted>
{
public:
    enum Type
    {
        unspecified = 0,
        generic = 1,
        nt = 2,
        win32 = 3
    };

    typedef RefCountedPtr<Error> Ref;

    static Ref make(
        const std::wstring& message,
        long code = 0,
        const char* file = nullptr,
        int line = 0,
        Ref inner = nullptr
        ) noexcept
    {
        return Ref(new Error(
            unspecified,
            code,
            message,
            file,
            line,
            inner
            ));
    }

    Type type() const noexcept
    {
        return m_type;
    }

    virtual const wchar_t* typeName() const noexcept
    {
        return L"Error";
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
        Type type,
        long code,
        const std::wstring& message,
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
    Type m_type;
    long m_code;
    std::wstring m_message;
    const char* m_file;
    int m_line;
    Ref m_inner;
};


class GenericError final
    : public Error
{
public:
    enum
    {
        Success = 0,
        EUnknown = -1,
        EOutOfMemory = -2,
        ETimedOut = -3,
        EIvalidArg = -4,
        ENotImplemented = -5,
        EInvalidRequest = -6
    };

    const wchar_t* typeName() const noexcept override
    {
        return L"GenericError";
    }

    std::wstring errorText() const noexcept override;

    static Ref make(
        long code,
        const std::wstring& message = std::wstring(),
        const char* file = nullptr,
        int line = 0,
        Ref inner = nullptr
        ) noexcept
    {
        return Ref(new GenericError(
            code,
            message,
            file,
            line,
            inner
            ));
    }

private:
    GenericError(
        long code,
        const std::wstring& message,
        const char* file,
        int line,
        Ref inner
        ) noexcept
        : Error(Error::generic, code, message, file, line, inner)
    {
    }
};

__forceinline bool isWin32ErrorStatus(HRESULT r) noexcept
{
    return ((HRESULT)r < 0);
}

std::wstring getWin32ErrorText(HRESULT r) noexcept;

__forceinline HRESULT lastError() noexcept
{
    return HRESULT_FROM_WIN32(::GetLastError());
}


class Win32Error final
    : public Error
{
public:
    const wchar_t* typeName() const noexcept override
    {
        return L"win32::Error";
    }

    std::wstring errorText() const noexcept override
    {
        return getWin32ErrorText(code());
    }

    static Ref make(
        HRESULT code,
        const std::wstring& message = std::wstring(),
        const char* file = nullptr,
        int line = 0,
        Ref inner = nullptr
        ) noexcept
    {
        return Ref(new Win32Error(
            code,
            message,
            file,
            line,
            inner
            ));
    }

private:
    Win32Error(
        long code,
        const std::wstring& message,
        const char* file,
        int line,
       Error::Ref inner
        ) noexcept
        : Error(Error::win32, code, message, file, line, inner)
    {
    }
};

__forceinline bool isNtErrorStatus(NTSTATUS r) noexcept
{
    return !NT_SUCCESS(r);
}

std::wstring getNtErrorText(NTSTATUS r) noexcept;


class NtError final
    : public Error
{
public:
    const wchar_t* typeName() const noexcept override
    {
        return L"nt::Error";
    }

    std::wstring errorText() const noexcept override
    {
        return getNtErrorText(code());
    }

    static Ref make(
        NTSTATUS code,
        const std::wstring& message = std::wstring(),
        const char* file = nullptr,
        int line = 0,
        Ref inner = nullptr
        ) noexcept
    {
        return Ref(new NtError(
            code,
            message,
            file,
            line,
            inner
            ));
    }

private:
    NtError(
        long code,
        const std::wstring& message,
        const char* file,
        int line,
        Error::Ref inner
        ) noexcept
        : Error(Error::nt, code, message, file, line, inner)
    {
    }
};


} // namespace Util {}

#pragma once

#include <windows.h>

#include "../Error.hxx"
#include "../../Util/Strings.hxx"

namespace Core
{

namespace Win32
{

inline bool isErrorStatus(HRESULT r) noexcept
{
    return ((HRESULT)r < 0);
}

inline std::wstring getWin32ErrorText(HRESULT r) noexcept
{
    if (r == S_OK)
    {
        return std::wstring();
    }

    return Util::formatMessage(r);
}

inline HRESULT lastError() noexcept
{
    return HRESULT_FROM_WIN32(::GetLastError());
}


class Error final
    : public Core::Error
{
public:
    enum
    {
        Type = 2
    };

    std::wstring errorText() const noexcept override
    {
        return getWin32ErrorText(code());
    }

    static Ref make(
        HRESULT code,
        const wchar_t* message = L"",
        const char* file = nullptr,
        int line = 0,
        Ref inner = nullptr
        )
    {
        return Ref(new Error(
            code,
            message,
            file,
            line,
            inner
            ));
    }

private:
    Error(
        long code,
        const wchar_t* message,
        const char* file,
        int line,
        Error::Ref inner
        )
        : Core::Error(Type, code, message, file, line, inner)
    {
    }
};


} // namespace Win32 {}

} // namespace Core {}

#include <windows.h>

#include "./Nt.hxx"
#include "./ntstatus.inl"
#include "../Error.hxx"

namespace Core
{

namespace Nt
{

inline bool isNtErrorStatus(NTSTATUS r) noexcept
{
    return !NT_SUCCESS(r);
}

inline std::wstring getErrorText(NTSTATUS r) noexcept
{
    return std::wstring(lookupNTSTATUSName(r));
}


class Error final
    : public Core::Error
{
public:
    enum
    {
        Type = 3
    };

    std::wstring errorText() const noexcept override
    {
        return getErrorText(code());
    }

    static Ref make(
        NTSTATUS code,
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


} // namespace Nt {}

} // namespace Core {}


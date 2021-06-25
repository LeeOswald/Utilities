#pragma once

#include "./Exception.hxx"
#include "./Win32Handle.hxx"

namespace Util
{

class Win32Event final
    : public Win32Handle
{
public:
    enum class Type
    {
        ManualReaset,
        AutoReset
    };

    enum 
    {
        WaitInfinite = INFINITE
    };

    Win32Event(Type t, const wchar_t* Name) noexcept
        : Win32Handle(::CreateEventW(nullptr, (t == Type::ManualReaset), false, Name))
    {
        if (!m_h)
            throw Exception(Win32Error::make(::GetLastError(), std::wstring(), __FILE__, __LINE__));
    }

    void Set() noexcept
    {
        ::SetEvent(m_h);
    }

    void Reset() noexcept
    {
        ::ResetEvent(m_h);
    }

    bool Wait(int Milliseconds = WaitInfinite) noexcept
    {
        auto r = ::WaitForSingleObject(m_h, Milliseconds);
        return (r == WAIT_OBJECT_0) || (r == WAIT_ABANDONED);
    }
};


} // namespace Util {}

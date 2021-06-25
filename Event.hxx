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

    enum class Result : uint32_t
    {
        Success = WAIT_OBJECT_0,
        Abandoned = WAIT_ABANDONED,
        Timeout = WAIT_TIMEOUT,
        Failed = WAIT_FAILED,
        Alerted = WAIT_IO_COMPLETION
    };

    Win32Event(Type t, const wchar_t* name = nullptr, bool Open = false)
        : Win32Handle(
        Open ?
        ::OpenEventW(SYNCHRONIZE, FALSE, name) :
        ::CreateEventW(nullptr, (t == Type::ManualReaset), false, name)
        )
    {
        if (!m_h)
            throw Exception(Win32Error::make(::GetLastError(), std::wstring(), __FILE__, __LINE__));
    }

    void Set()
    {
        if (!::SetEvent(m_h))
            throw Exception(Win32Error::make(::GetLastError(), std::wstring(), __FILE__, __LINE__));
    }

    void Reset()
    {
        if (!::ResetEvent(m_h))
            throw Exception(Win32Error::make(::GetLastError(), std::wstring(), __FILE__, __LINE__));
    }

    Result Wait(int Milliseconds = WaitInfinite)
    {
        auto r = static_cast<Result>(::WaitForSingleObjectEx(m_h, Milliseconds, TRUE));
        if (r == Result::Failed)
            throw Exception(Win32Error::make(::GetLastError(), std::wstring(), __FILE__, __LINE__));

        return r;
    }
};


} // namespace Util {}

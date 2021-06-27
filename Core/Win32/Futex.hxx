#pragma once

#include <windows.h>


namespace Core
{

namespace Win32
{

class Futex final
{
public:
    ~Futex() noexcept
    {
        ::DeleteCriticalSection(&m_cs);
    }

    Futex() noexcept
    {
        ::InitializeCriticalSectionAndSpinCount(&m_cs, 4000);
    }

    Futex(const Futex&) = delete;
    Futex& operator=(const Futex&) = delete;

    void lock() noexcept
    {
        ::EnterCriticalSection(&m_cs);
    }

    bool try_lock() noexcept
    {
        return !!::TryEnterCriticalSection(&m_cs);
    }

    void unlock() noexcept
    {
        ::LeaveCriticalSection(&m_cs);
    }

private:
    CRITICAL_SECTION m_cs;
};

} // namespace Win32{}

} // namespace Core {}

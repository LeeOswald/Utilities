#pragma once

#include <windows.h>

namespace Util
{

class FastMutex final
{
public:
    ~FastMutex() noexcept
    {
        ::DeleteCriticalSection(&m_cs);
    }

    FastMutex() noexcept
    {
        ::InitializeCriticalSectionAndSpinCount(&m_cs, 4000);
    }

    FastMutex(const FastMutex&) = delete;
    FastMutex& operator=(const FastMutex&) = delete;

    void lock() noexcept
    {
        ::EnterCriticalSection(&m_cs);
    }

    void unlock() noexcept
    {
        ::LeaveCriticalSection(&m_cs);
    }

private:
    CRITICAL_SECTION m_cs;
};


class LockGuard
{
public:
    ~LockGuard() noexcept
    {
        m_mtx.unlock();
    }

    LockGuard(FastMutex& mtx)
        : m_mtx(mtx)
    {
    }

    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;

private:
    FastMutex& m_mtx;
};

} // namespace Util {}

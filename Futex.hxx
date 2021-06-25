#pragma once

#include <windows.h>

namespace Util
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

    LockGuard(Futex& mtx)
        : m_mtx(mtx)
    {
    }

    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;

private:
    Futex& m_mtx;
};

} // namespace Util {}

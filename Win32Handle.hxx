#pragma once

#include <windows.h>

#include <utility>

namespace Util
{

struct Win32Handle
{
    ~Win32Handle() noexcept
    {
        if (m_h && (m_h != INVALID_HANDLE_VALUE))
            ::CloseHandle(m_h);
    }

    Win32Handle(HANDLE h = INVALID_HANDLE_VALUE) noexcept
        : m_h(h)
    {}

    operator HANDLE() const noexcept
    {
        return m_h;
    }

    bool valid() const noexcept
    {
        return (m_h && (m_h != INVALID_HANDLE_VALUE));
    }

    Win32Handle(Win32Handle const&) = delete;
    Win32Handle& operator=(Win32Handle const&) = delete;

    void swap(Win32Handle& o) noexcept
    {
        std::swap(m_h, o.m_h);
    }

    Win32Handle(Win32Handle&& o) noexcept
        : Win32Handle()
    {
        o.swap(*this);
    }

    Win32Handle& operator=(Win32Handle&& o) noexcept
    {
        if (&o != this)
        {
            Win32Handle t(std::move(o));
            t.swap(*this);
        }

        return *this;
    }

    HANDLE detach() noexcept
    {
        auto h = m_h;
        m_h = INVALID_HANDLE_VALUE;
        return h;
    }

protected:
    HANDLE m_h;
};


} // namespace Util {}

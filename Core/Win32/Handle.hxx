#pragma once

#include <windows.h>

#include <utility>

namespace Core
{

namespace Win32
{

struct Handle
{
    ~Handle() noexcept
    {
        if (m_h && (m_h != INVALID_HANDLE_VALUE))
            ::CloseHandle(m_h);
    }

    Handle(HANDLE h = INVALID_HANDLE_VALUE) noexcept
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

    Handle(Handle const&) = delete;
    Handle& operator=(Handle const&) = delete;

    void swap(Handle& o) noexcept
    {
        std::swap(m_h, o.m_h);
    }

    Handle(Handle&& o) noexcept
        : Handle()
    {
        o.swap(*this);
    }

    Handle& operator=(Handle&& o) noexcept
    {
        if (&o != this)
        {
            Handle t(std::move(o));
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

inline void swap(Handle& a, Handle& b) noexcept
{
    a.swap(b);
}

} // namespace Win32 {}

} // namespace Core {}

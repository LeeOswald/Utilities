#pragma once

#include <cstdint>
#include <functional>

#include "./Handle.hxx"
#include "../Thread.hxx"


namespace Core
{

namespace Win32
{

namespace CurrentThread
{

inline uint32_t id() noexcept
{
    return ::GetCurrentThreadId();
}

inline HANDLE handle() noexcept
{
    return ::GetCurrentThread();
}

inline void yield() noexcept
{
    ::Sleep(0);
}

inline void sleep(uint32_t Milliseconds) noexcept
{
    ::Sleep(Milliseconds);
}

inline bool sleepAlertable(uint32_t Milliseconds) noexcept // returns true if alerted
{
    return (::SleepEx(Milliseconds, TRUE) == WAIT_IO_COMPLETION);
}

void setPriority(Core::ThreadPriority p) noexcept;

inline void setAffinity(uintptr_t a) noexcept
{
    ::SetThreadAffinityMask(::GetCurrentThread(), a);
}

inline uint32_t currentProcessor() noexcept
{
    return ::GetCurrentProcessorNumber();
}

void setName(const char* name) noexcept;

} // namespace CurrentThread {}



class Thread final
    : public Handle
{
public:
    using Task = std::function<void(void*)>;

    ~Thread();
    Thread();
    Thread(Task&& task, void* ctx, const char* name = nullptr);

    Thread(Thread&& o) noexcept
        : Thread()
    {
        o.swap(*this);
    }

    Thread& operator=(Thread&& o) noexcept
    {
        if (&o != this)
        {
            Thread t(std::move(o));
            t.swap(*this);
        }

        return *this;
    }

    uint32_t id() const noexcept
    {
        return m_id;
    }

    HANDLE handle() const noexcept
    {
        return m_h;
    }

    void alert() noexcept;
    void join() noexcept;

    void swap(Thread& o) noexcept;

private:
    static void CALLBACK _APCProc(ULONG_PTR dwParam) noexcept;
    static unsigned __stdcall _threadProcStatic(void* arg);
    void _run();

    uint32_t m_id;
    Task m_task;
    void* m_ctx;
    char m_name[32];
    bool m_joined;
};



} // namespace Win32 {}

} // namespace Core {}

#include "../Exception.hxx"
#include "../Thread.hxx"
#include "./Error.hxx"

#include <cassert>
#include <process.h>

namespace Core
{

namespace Win32
{

namespace
{

const DWORD VCThreadNameMagic = 0x406D1388;
#pragma pack(push,8)
typedef struct
{
    DWORD dwType;      // Must be 0x1000.
    LPCSTR szName;     // Pointer to name (in user addr space).
    DWORD dwThreadID;  // Thread ID (-1=caller thread).
    DWORD dwFlags;     // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

static void setNameInternal(DWORD tid, const char* name)
{
    if (::IsDebuggerPresent())
    {
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = name;
        info.dwThreadID = tid;
        info.dwFlags = 0;

        __try
        {
            ::RaiseException(VCThreadNameMagic, 0, sizeof(info) / sizeof(DWORD), reinterpret_cast<DWORD_PTR*>(&info));
        }
        __except (EXCEPTION_CONTINUE_EXECUTION)
        {
        }
    }
}

}

namespace CurrentThread
{

void setPriority(ThreadPriority p) noexcept
{
    int pr = 0;
    switch (p)
    {
    case ThreadPriority::low: pr = THREAD_PRIORITY_BELOW_NORMAL; break;
    case ThreadPriority::normal: pr = THREAD_PRIORITY_NORMAL; break;
    case ThreadPriority::high: pr = THREAD_PRIORITY_ABOVE_NORMAL; break;
    case ThreadPriority::realtime: pr = THREAD_PRIORITY_TIME_CRITICAL; break;
    default:
        assert(0);
    }

    ::SetThreadPriority(::GetCurrentThread(), pr);
}

void setName(const char* name) noexcept
{
    if (name)
    {
        setNameInternal((DWORD)-1, name);
    }
}

} // namespace CurrentThread {}


Thread::~Thread()
{
    join();
}

Thread::Thread()
    : Handle(0)
    , m_id(0)
    , m_ctx(nullptr)
    , m_joined(false)
{
}

Thread::Thread(Task&& task, void* ctx, const char* name)
    : Handle(0)
    , m_id(0)
    , m_task(std::move(task))
    , m_ctx(ctx)
    , m_joined(false)
{
    if (name)
    {
        ::strncpy_s(m_name, _countof(m_name), name, _TRUNCATE);
        m_name[_countof(m_name) - 1] = 0;
    }
    else
    {
        m_name[0] = 0;
    }

    m_h = (HANDLE)::_beginthreadex(nullptr, 0, _threadProcStatic, this, 0, &m_id);
    if (!m_h)
        Exception(Win32::Error::make(::GetLastError(), L"", __FILE__, __LINE__));
}

void Thread::_APCProc(ULONG_PTR) noexcept
{
}

unsigned __stdcall Thread::_threadProcStatic(void* arg)
{
    auto _this = static_cast<Thread*>(arg);
    _this->_run();
    return 0;
}

void Thread::_run()
{
    if (m_name[0])
    {
        CurrentThread::setName(m_name);
    }

    m_task(m_ctx);
}

void Thread::alert() noexcept
{
    if (m_h)
        ::QueueUserAPC(_APCProc, m_h, 0);
}

void Thread::join() noexcept
{
    if (!m_h)
        return;

    if (m_joined)
        return;

    assert(CurrentThread::id() != id());
    if (CurrentThread::id() == id())
    {
        return;
    }

    alert();

    ::WaitForSingleObject(m_h, INFINITE);
    m_joined = true;
}

void Thread::swap(Thread& o) noexcept
{
    using std::swap;
    Handle::swap(o);
    swap(m_id, o.m_id);
    swap(m_task, o.m_task);
    swap(m_ctx, o.m_ctx);
    char n[_countof(m_name)];
    ::memcpy(m_name, n, sizeof(m_name));
    ::memcpy(m_name, o.m_name, sizeof(m_name));
    ::memcpy(o.m_name, n, sizeof(m_name));

    swap(m_joined, o.m_joined);
}

} // namespace Win32 {}

} // namespace Core

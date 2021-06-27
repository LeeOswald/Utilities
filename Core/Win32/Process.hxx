#include <windows.h>

#include <cstdint>

namespace Core
{

namespace Win32
{

namespace CurrentProcess
{

inline uint32_t id()
{
    return ::GetCurrentProcessId();
}

inline HANDLE handle()
{
    return ::GetCurrentProcess();
}


} // namespace CurrentProcess {}


} // namespace Win32 {}

} // namespace Core {}

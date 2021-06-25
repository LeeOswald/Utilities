#pragma once

#include <cstdint>

#include <windows.h>

namespace Util
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

} // namespace Util {}

#pragma once

namespace Core
{

enum class ThreadPriority : int
{
    low,
    normal,
    high,
    realtime
};

} // namespace Core {}


#include "./Win32/Thread.hxx"

namespace Core
{

namespace CurrentThread = Win32::CurrentThread;

using Thread = Win32::Thread;

} // namespace Core {}


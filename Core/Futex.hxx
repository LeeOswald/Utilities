#pragma once

#include <mutex>

#include "./Win32/Futex.hxx"

namespace Core
{

using Futex = Win32::Futex;

} // namespace Core {}

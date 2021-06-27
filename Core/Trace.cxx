#include "./Futex.hxx"
#include "./Trace.hxx"
#include "../Util/IntrusiveList.hxx"
#include "../Util/Strings.hxx"

#include <condition_variable>
#include <cstdio>
#include <iostream>
#include <mutex>


namespace Core
{

namespace Trace
{

Level g_Level = Debug;

namespace
{

const int kMaxIndent = 64;
bool g_Enabled = false;
bool g_Console = false;
__declspec(thread) int g_Indent = 0;
std::mutex g_Lock;
std::condition_variable g_DataAvailable;
bool g_Stop = false;


void bailOutWrite(const wchar_t* text, va_list args)
{
    static wchar_t buffer[1024];
    ::_vsnwprintf_s(buffer, _countof(buffer), _TRUNCATE, text, args);

    std::lock_guard<std::mutex> l(g_Lock);
    ::OutputDebugStringW(buffer);
    ::OutputDebugStringW(L"\n");

    if (g_Console)
        std::wcout << buffer << std::endl;
}

} // namespace {}



void initialize(bool console)
{
    if (g_Enabled)
        return;

    g_Stop = false;
    g_Console = console;
    g_Enabled = true;
}

void finaliize()
{
    if (!g_Enabled)
        return;

    g_Stop = true;
    g_DataAvailable.notify_one();
}

void registerSink(ITraceSink* sink)
{

}

void unregisterSink(ITraceSink* sink)
{


}

void writeV(Level level, const char* module, const char* file, int line, const wchar_t* text, va_list args)
{
    if (level < g_Level)
        return;

    if (!g_Enabled)
        return;

    if (!module)
        module = "MAIN";

    if (!file)
        file = "n/a";

    try
    {
        auto s = Util::format(text, args);
        auto r = Record::make(level, module, file, line, std::move(text));
    }
    catch (std::bad_alloc&)
    {
        bailOutWrite(text, args);
    }
}

int indent(int delta)
{
    std::lock_guard<std::mutex> l(g_Lock);

    auto i = g_Indent;
    g_Indent += delta;
    if (g_Indent < 0)
        g_Indent = 0;
    else if (g_Indent > kMaxIndent)
        g_Indent = kMaxIndent;

    return i;
}

void setIndent(int indent)
{
    std::lock_guard<std::mutex> l(g_Lock);

    g_Indent = indent;
    if (g_Indent < 0)
        g_Indent = 0;
    else if (g_Indent > kMaxIndent)
        g_Indent = kMaxIndent;
}

} // namespace Trace {}

} // namespace Core {}

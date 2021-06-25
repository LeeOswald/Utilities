#pragma once

#include "./IRefCounted.hxx"
#include "./Process.hxx"
#include "./Thread.hxx"
#include "./Time.hxx"

#include <string>

namespace Util
{

namespace Trace
{

enum Level
{
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Highest = 4,
    Off // should go last
};

extern Level g_Level;

class Record
    : public RefCountedBase<IRefCounted>
{
public:
    typedef RefCountedPtr<Record> Ref;

    static inline Ref make(
        Level level,
        const char* module,
        const char* file,
        int line,
        std::wstring&& text
        )
    {
        return Ref(new Record(
            level,
            module,
            file,
            line,
            Time::now(),
            CurrentProcess::id(),
            CurrentThread::id(),
            std::move(text)
            ));
    }

    inline Level level() const noexcept
    {
        return m_level;
    }

    inline const char* module() const noexcept
    {
        return m_module;
    }

    inline const char* file() const noexcept
    {
        return m_file;
    }

    inline int line() const noexcept
    {
        return m_line;
    }

    inline Time time() const noexcept
    {
        return m_time;
    }

    inline uint32_t pid() const noexcept
    {
        return m_pid;
    }

    inline uint32_t tid() const noexcept
    {
        return m_tid;
    }

    inline const std::wstring& text() const noexcept
    {
        return m_text;
    }

protected:
    virtual ~Record() noexcept
    {
    }

    Record(
        Level level,
        const char* module,
        const char* file,
        int line,
        Time time,
        uint32_t pid,
        uint32_t tid,
        std::wstring&& text
        )
        : m_level(level)
        , m_module(module)
        , m_file(file)
        , m_line(line)
        , m_time(time)
        , m_pid(pid)
        , m_tid(tid)
        , m_text(std::move(text))
    {
    }

private:
    Level m_level;
    const char* m_module;
    const char* m_file;
    int m_line;
    Time m_time;
    uint32_t m_pid;
    uint32_t m_tid;
    std::wstring m_text;
};


struct __declspec(novtable) ITraceSink
{
    virtual void write(Record::Ref r) noexcept = 0;
};


void initialize(bool console);
void finaliize();

void registerSink(ITraceSink* sink);
void unregisterSink(ITraceSink* sink);

void writeV(Level level, const char* module, const char* file, int line, const wchar_t* text, va_list args);

inline bool writeDebug(const char* module, const char* file, int line, const wchar_t* format, ...) noexcept
{
    va_list args;
    va_start(args, format);
    writeV(Debug, module, file, line, format, args);
    va_end(args);
    return true;
}

inline bool writeInfo(const char* module, const char* file, int line, const wchar_t* format, ...) noexcept
{
    va_list args;
    va_start(args, format);
    writeV(Info, module, file, line, format, args);
    va_end(args);
    return true;
}

inline bool writeWarning(const char* module, const char* file, int line, const wchar_t* format, ...) noexcept
{
    va_list args;
    va_start(args, format);
    writeV(Warning, module, file, line, format, args);
    va_end(args);
    return true;
}

inline bool writeError(const char* module, const char* file, int line, const wchar_t* format, ...) noexcept
{
    va_list args;
    va_start(args, format);
    writeV(Error, module, file, line, format, args);
    va_end(args);
    return true;
}

inline bool writeCritical(const char* module, const char* file, int line, const wchar_t* format, ...) noexcept
{
    va_list args;
    va_start(args, format);
    writeV(Highest, module, file, line, format, args);
    va_end(args);
    return true;
}

int indent(int delta);
void setIndent(int indent);

class IndentScope
{
public:
    IndentScope(int delta)
        : m_indent(indent(delta))
    {
    }

    ~IndentScope()
    {
        setIndent(m_indent);
    }

private:
    int m_indent;
};





} // namespace Trace {}

} // namespace Util {}

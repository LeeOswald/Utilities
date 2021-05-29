#pragma once

#ifndef __MOROSE_TRACE_H_INCLUDED__
#define __MOROSE_TRACE_H_INCLUDED__

#include <atomic>
#include <cstdarg>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>


#include <windows.h>

#include "strings.h"


#undef __MOROSE_USE_FILE
#define __MOROSE_USE_FILE      0       // output to UTF-8 encoded file

#undef __MOROSE_USE_DEBUGGER
#define __MOROSE_USE_DEBUGGER  1      // output to the debugger window

#undef __MOROSE_USE_CONSOLE
#define __MOROSE_USE_CONSOLE   1

#undef __MOROSE_LOG_PREFIX
#define __MOROSE_LOG_PREFIX L"Morose"  // prefix for log file names

#undef __MOROSE_USE_RECORD_PREFIXES    // output timestamps
#define __MOROSE_USE_RECORD_PREFIXES 1

#undef __MOROSE_MAX_SAFE_BUFFER
#define  __MOROSE_MAX_SAFE_BUFFER 4096

#undef __MOROSE_INDENT_SIZE
#define __MOROSE_INDENT_SIZE 3

#undef __MOROSE_MAX_INDENT
#define __MOROSE_MAX_INDENT 64


namespace trace
{

namespace detail
{

#pragma region safe_buffer

template <typename T>
struct safe_buffer;

template<>
struct safe_buffer<void>
{
};

template <typename T>
struct safe_buffer
{
public:
	static safe_buffer& instance() noexcept
	{
		static safe_buffer s;
		return s;
	}

	struct buffer
	{
	public:
		explicit buffer(T* buf, size_t size, std::mutex& mtx) noexcept
			: buffer_(buf)
			, size_(size)
			, lock_(&mtx)
		{
			lock_->lock();
		}

		~buffer() noexcept
		{
			if (lock_)
				lock_->unlock();
		}

		buffer(const buffer&) = delete;
		buffer& operator =(const buffer&) = delete;

		void swap(buffer& o) noexcept
		{
			using std::swap;
			swap(buffer_, o.buffer_);
			swap(size_, o.size_);
			swap(lock_, o.lock_);
		}

		buffer(buffer&& o) noexcept
			: buffer()
		{
			o.swap(this);
		}

		buffer&  operator=(buffer&& o) noexcept
		{
			if (this != &o)
			{
				buffer(std::move(o)).swap(this);
			}

			return *this;
		}

		std::pair<T*, size_t> get() noexcept
		{
			return std::make_pair(buffer_, size_t(size_ / sizeof(T)));
		}

	private:
		buffer() noexcept {}

		T* buffer_= nullptr;
		size_t size_ = 0;
		std::mutex *lock_ = nullptr;
	};

	safe_buffer(const safe_buffer&) = delete;
	safe_buffer& operator==(const safe_buffer&) = delete;

	buffer get() noexcept
	{
		return buffer(_buffer(), __MOROSE_MAX_SAFE_BUFFER * sizeof(T), _lock());
	}

private:
	safe_buffer() noexcept {}

	static T* _buffer() noexcept
	{
		static T b[__MOROSE_MAX_SAFE_BUFFER];
		return b;
	}
	
	static std::mutex& _lock() noexcept
	{
		static std::mutex m;
		return m;
	}
};

#pragma endregion // safe_buffer

#pragma region trace_context

struct trace_context
{
public:
	static trace_context& instance() noexcept
	{
		static trace_context s;
		return s;
	}

	struct writer
	{
	public:
#if !__MOROSE_USE_FILE 
		explicit writer(std::mutex& mtx) noexcept
			: lock_(&mtx)
		{
			lock_->lock();
		}
#else
		explicit writer(HANDLE h, std::mutex& mtx) noexcept
			: file_(h)
			, lock_(&mtx)
		{
			lock_->lock();
		}
#endif
		~writer() noexcept
		{
			if (lock_)
				lock_->unlock();
		}

		writer(const writer&) = delete;
		writer& operator =(const writer&) = delete;

		void swap(writer& o) noexcept
		{
			using std::swap;
#if __MOROSE_USE_FILE 
			swap(file_, o.file_);
#endif
			swap(lock_, o.lock_);
		}

		writer(writer&& o) noexcept
			: writer()
		{
			o.swap(*this);
		}

		writer& operator=(writer&& o) noexcept
		{
			if (this != &o)
				writer(std::move(o)).swap(*this);

			return *this;
		}

		void write(const wchar_t* ws, size_t length)
		{
			if (length < 0)
				length = std::wcslen(ws);

#if __MOROSE_USE_FILE 
			auto s = strings::ws2utf8(std::wstring(ws, length));
			DWORD wr = 0;
			::WriteFile(_file(), s.c_str(), static_cast<DWORD>(s.length()), &wr, 0);
#endif

#if __MOROSE_USE_DEBUGGER
			if (::IsDebuggerPresent())
				::OutputDebugStringW(ws);
#endif

#if __MOROSE_USE_CONSOLE
			std::wcout << ws;
#endif
		}

		void write(const std::wstring& s)
		{
			write(s.data(), s.length());
		}

		void write_s(const wchar_t* s, size_t length)
		{
			if (length < 0)
				length = std::wcslen(s);

			if (!length)
				return;

#if __MOROSE_USE_FILE 
			{
				// convert to UTF-8
				auto sb = safe_buffer<char>::instance().get();;
				auto b = sb.get();
				auto result = ::WideCharToMultiByte(CP_UTF8, 0, s, static_cast<int>(length), b.first, static_cast<int>(b.second), nullptr, nullptr);
				if (!result)
					return;

				DWORD wr = 0;
				::WriteFile(_file(), b.first, static_cast<DWORD>(result), &wr, 0);
			}
#endif

#if __MOROSE_USE_DEBUGGER
			if (::IsDebuggerPresent())
				::OutputDebugStringW(s);
#endif

#if __MOROSE_USE_CONSOLE
			std::wcout << s;
#endif
		}

		void write_s(const std::wstring& s)
		{
			write(s.data(), s.length());
		}

		void flush()
		{
#if __MOROSE_USE_FILE
			::FlushFileBuffers(_file());
#endif
		}

	private:
		writer() noexcept {}

#if __MOROSE_USE_FILE 
		HANDLE file_ = INVALID_HANDLE_VALUE;
#endif
		std::mutex* lock_ = nullptr;
	};

	trace_context(const trace_context&) = delete;
	trace_context& operator=(const trace_context&) = delete;

	writer get() noexcept
	{
#if __MOROSE_USE_FILE 
		writer w(_file(), _lock());
#else
		writer w(_lock());
#endif
		return w;
	}

private:
	trace_context()
	{
#if __MOROSE_USE_FILE 
		wchar_t temp_path[MAX_PATH];
		::GetTempPathW(_countof(temp_path), temp_path);

		SYSTEMTIME st = { 0 };
		::GetLocalTime(&st);
		wchar_t path[MAX_PATH];
		::_snwprintf_s(
			path,
			_countof(path),
			L"%s%s%02d%02d%02d%03d.log",
			temp_path,
			__MOROSE_LOG_PREFIX,
			st.wHour,
			st.wMinute,
			st.wSecond,
			st.wMilliseconds
			);

		_file() = ::CreateFileW(path, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (_file() == INVALID_HANDLE_VALUE)
			throw std::exception("Failed to create a trace file");
#endif
	}

	~trace_context() noexcept
	{
#if __MOROSE_USE_FILE
		if (_file())
		{
			::CloseHandle(_file());
			_file() = INVALID_HANDLE_VALUE;
		}
#endif __MOROSE_USE_FILE
	}

	static std::mutex& _lock() noexcept
	{
		static std::mutex m;
		return m;
	}

#if __MOROSE_USE_FILE 
	static HANDLE& _file() noexcept
	{
		static HANDLE f = INVALID_HANDLE_VALUE;
		return f;
	}
#endif

};

#pragma endregion // trace_constext

} // namespace detail {}

static inline int* _indent() noexcept
{
	thread_local int i = 0;
	return& i;
}

inline int current_indent() noexcept
{
	return *_indent();
}

inline int change_indent(int delta) noexcept
{
	auto current = _indent();

	*current += delta;
	if (*current > __MOROSE_MAX_INDENT)
		*current = __MOROSE_MAX_INDENT;
	if (*current < 0)
		*current = 0;

	return *current;
}

#pragma region trace_s

inline void traceV_s(const wchar_t* format, va_list args) noexcept
{
	auto& tc = detail::trace_context::instance();
	auto w = tc.get();

	// prefix
#if	__MOROSE_USE_RECORD_PREFIXES
	{
		SYSTEMTIME st = { 0 };
		::GetLocalTime(&st);
		wchar_t Prefix[64];
		::_snwprintf_s(
			Prefix,
			_countof(Prefix),
			L"[%02d:%02d:%02d.%03d %d:%d] ",
			st.wHour,
			st.wMinute,
			st.wSecond,
			st.wMilliseconds,
			::GetCurrentProcessId(),
			::GetCurrentThreadId()
			);

		w.write_s(Prefix);
	}
#endif

	// indent
	{
		wchar_t indent[__MOROSE_MAX_INDENT];
		for (int i = 0; i < current_indent(); ++i)
			indent[i] = L' ';
		indent[current_indent()] = L'\0';
		w.write_s(indent, current_indent());
	}

	// actual text
	const wchar_t* text = nullptr;
	int length = 0;
	{
		auto sb = detail::safe_buffer<wchar_t>::instance().get();
		auto b = sb.get();
		length = ::_vsnwprintf_s(b.first, b.second, _TRUNCATE, format, args);

		if (length > 0)
			w.write_s(b.first, length);
	}

	w.write_s(L"\n", 1);
}

inline void trace_s(const wchar_t* format, ...) noexcept
{
	va_list args;
	va_start(args, format);

	traceV_s(format, args);

	va_end(args);
}

#pragma endregion // trace_s

#pragma region trace

inline void traceV(const wchar_t* format, va_list args) noexcept
{
	auto s = strings::formatV(format, args);
	s.append(L"\n");

	auto ind = current_indent();
	if (ind > 0)
	{
		std::wstring i;
		i.append(ind, L' ');

		s = i + s;
	}

#if __MOROSE_USE_RECORD_PREFIXES
	{
		SYSTEMTIME st = { 0 };
		::GetLocalTime(&st);
		wchar_t Prefix[64];
		::_snwprintf_s(
			Prefix,
			_countof(Prefix),
			_TRUNCATE,
			L"[%02d:%02d:%02d.%03d %d:%d] ",
			st.wHour,
			st.wMinute,
			st.wSecond,
			st.wMilliseconds,
			::GetCurrentProcessId(),
			::GetCurrentThreadId()
			);
		s = std::wstring(Prefix).append(s);
	}
#endif
	auto& tc = detail::trace_context::instance();
	auto w = tc.get();
	w.write(s);
}

inline void trace(const wchar_t* format, ...) noexcept
{
	va_list args;
	va_start(args, format);

	traceV(format, args);

	va_end(args);
}

#pragma endregion

inline void flush()
{
#if __MOROSE_USE_FILE 
	auto& tc = detail::trace_context::instance();
	auto c = tc.get();
	c.flush();
#endif __MOROSE_USE_FILE 
}

#pragma  endregion // trace

#pragma  region indent_scope_s

struct indent_scope_s
{
public:
	~indent_scope_s()
	{
		change_indent(-delta_);
	}

	explicit indent_scope_s(int delta = __MOROSE_INDENT_SIZE) noexcept
		: delta_(delta)
	{
		change_indent(delta);
	}

	explicit indent_scope_s(int delta, const wchar_t* format, ...) noexcept
		: delta_(delta)
	{
		va_list args;
		va_start(args, format);
		traceV_s(format, args);
		va_end(args);

		change_indent(delta);
	}

private:
	int delta_ = 0;
};

#pragma endregion // indent_scope_s


} // namespace trace {}

#endif // __MOROSE_TRACE_H_INCLUDED__
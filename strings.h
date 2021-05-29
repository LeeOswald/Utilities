#pragma once

#ifndef __MOROSE_STRINGS_H_INCLUDED__
#define __MOROSE_STRINGS_H_INCLUDED__

#include <malloc.h>
#include <memory>
#include <string>

#include <windows.h>

namespace strings
{

inline std::wstring s2ws(unsigned int cp, const char* s, size_t length = size_t(-1))
{
	// required buffer length
	auto required = ::MultiByteToWideChar(cp, 0, s, static_cast<int>(length), nullptr, 0);
	if (!required)
		return std::wstring();

	std::wstring buffer(required, 'L\0');
	auto result = ::MultiByteToWideChar(cp, 0, s, static_cast<int>(length), const_cast<wchar_t*>(buffer.data()), static_cast<int>(buffer.capacity()));
	if (!result)
		return std::wstring();

	return std::wstring(buffer);
}

inline std::wstring s2ws(unsigned int cp, const std::string& s)
{
	return s2ws(cp, s.data(), s.length());
}

inline std::wstring ansi2ws(const std::string& s)
{
	return s2ws(CP_THREAD_ACP, s);
}

inline std::wstring utf82ws(const std::string& s)
{
	return s2ws(CP_UTF8, s);
}

inline std::string ws2s(unsigned int cp, const wchar_t* s, size_t length = size_t(-1))
{
	// required buffer length
	auto required = ::WideCharToMultiByte(cp, 0, s, static_cast<int>(length), nullptr, 0, nullptr, nullptr);
	if (!required)
		return std::string();

	std::string buffer(required, '\0');
	auto result = ::WideCharToMultiByte(cp, 0, s, static_cast<int>(length), const_cast<char*>(buffer.data()), static_cast<int>(buffer.capacity()), nullptr, nullptr);
	if (!result)
		return std::string();

	return std::string(buffer);
}

inline std::string ws2s(unsigned int cp, const std::wstring& s)
{
	return ws2s(cp, s.data(), s.length());

}
inline std::string ws2ansi(const std::wstring& s)
{
	return ws2s(CP_THREAD_ACP, s);
}

inline std::string ws2utf8(const std::wstring& s)
{
	return ws2s(CP_UTF8, s);
}

inline std::wstring formatV(const wchar_t* format, va_list args)
{
	auto n = static_cast<int>(::wcslen(format));

	std::unique_ptr<wchar_t[]> formatted;

	int result;
	do
	{
		n *= 2;
		formatted.reset(new wchar_t[n]);

		result = ::_vsnwprintf_s(formatted.get(), n, _TRUNCATE, format, args);

	} while (result < 0);

	return std::wstring(formatted.get());
}

inline std::wstring format(const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);

	auto s = formatV(format, args);

	va_end(args);

	return s;
}

inline void formatV_s(wchar_t* buffer, size_t max, const wchar_t* format, va_list args)
{
	::_vsnwprintf_s(buffer, max, _TRUNCATE, format, args);
}

inline void format_s(wchar_t* buffer, size_t max, const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);

	formatV_s(buffer ,max, format, args);

	va_end(args);
}

inline std::string formatV(const char* format, va_list args)
{
	auto n = static_cast<int>(::strlen(format));

	std::unique_ptr<char[]> formatted;

	int result;
	do
	{
		n *= 2;
		formatted.reset(new char[n]);

		result = ::vsnprintf_s(formatted.get(), n, _TRUNCATE, format, args);

	} while (result < 0);

	return std::string(formatted.get());
}

inline std::string format(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	auto s = formatV(format, args);

	va_end(args);

	return s;
}

inline void formatV_s(char* buffer, size_t max, const char* format, va_list args)
{
	::vsnprintf_s(buffer, max, _TRUNCATE, format, args);
}

inline void format(char* buffer, size_t max, const char* format, ...)
{
	va_list args;
	va_start(args, format);

	formatV_s(buffer, max, format, args);

	va_end(args);
}

inline std::wstring formatMessage(long r, HINSTANCE module = 0)
{
	if (r == 0) return std::wstring();

	uint32_t flags = module ?
		(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS) :
		(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS);
	
	wchar_t* buffer = nullptr;
	auto cch = ::FormatMessageW(
		flags,
		module,
		r,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
		reinterpret_cast<wchar_t*>(&buffer),
		0,
		nullptr
	);

	struct HeapFreeOperator
	{
		void operator()(wchar_t *p)
		{
			::HeapFree(::GetProcessHeap(), 0, p);
		}
	};

	std::unique_ptr<wchar_t[], HeapFreeOperator> bufferScope(buffer);
	std::wstring s;

	if (cch > 0)
	{
		s = std::wstring(buffer, cch);

		while (s.size() && (s[s.size() - 1] == L'\n' || s[s.size() - 1] == L'\r'))
		{
			s.erase(s.size() - 1);
		}

		if (s.size() && s[s.size() - 1] == L'.')
		{
			s.erase(s.size() - 1);
		}
	}
	else
	{
		s = std::to_wstring(r);
	}

	return s;
}


} // namespace strings {}

#endif // __MOROSE_STRINGS_H_INCLUDED__
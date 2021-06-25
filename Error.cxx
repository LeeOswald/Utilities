#include "./Error.hxx"
#include "./ntstatus.inl"
#include "./Strings.hxx"

#include <cassert>


namespace Util
{

std::wstring Error::format(const wchar_t* format) const noexcept
{
    assert(format);
    if (!format)
    {
        return std::wstring();
    }

    std::wstring result;

    auto p = format;
    while (*p)
    {
        if (*p ==L'{')
        {
            if (*(p + 1) ==L'{') // {{
            {
                result.append(L"{");
                p += 2;
            }
            else if ((*(p + 1) ==L'T') && (*(p + 2) ==L'}')) // {T}
            {
                result.append(typeName());
                p += 3;
            }
            else if ((*(p + 1) ==L'C') && (*(p + 2) ==L'd') && (*(p + 3) ==L'}')) // {Cd}
            {
                if (code() != 0)
                {
                    result.append(Util::format(L"%d", code()));
                }

                p += 4;
            }
            else if ((*(p + 1) ==L'C') && (*(p + 2) ==L'x') && (*(p + 3) ==L'}')) // {Cx}
            {
                if (code() != 0)
                {
                    result.append(Util::format(L"%08x", code()));
                }

                p += 4;
            }
            else if ((*(p + 1) ==L'M') && (*(p + 2) ==L'}')) // {M}
            {
                result.append(m_message);
                p += 3;
            }
            else if ((*(p + 1) ==L'S') && (*(p + 2) ==L'}')) // {S}
            {
                result.append(errorText());
                p += 3;
            }
            else if ((*(p + 1) ==L'M') && (*(p + 2) ==L'S') && (*(p + 3) ==L'}')) // {MS}
            {
                auto es = errorText();
                if (!m_message.empty() && !es.empty())
                {
                    result.append(m_message);
                    result.append(L" [");
                    result.append(es);
                    result.append(L"]");
                }
                else // append whatever is present
                {
                    result.append(m_message);
                    result.append(es);
                }

                p += 4;
            }
            else if ((*(p + 1) ==L'F') && (*(p + 2) ==L'}')) // {F}
            {
                if (m_file)
                {
                    result.append(s2ws(CP_THREAD_ACP, m_file));
                }

                p += 3;
            }
            else if ((*(p + 1) ==L'L') && (*(p + 2) ==L'}')) // {L}
            {
                if (m_line)
                {
                    result.append(Util::format(L"%d", m_line));
                }

                p += 3;
            }
            else if ((*(p + 1) ==L'F') && (*(p + 2) ==L'L') && (*(p + 3) ==L'}')) // {FL}
            {
                if (m_file)
                {
                    result.append(Util::format(L"%S:%d", m_file, m_line));
                }

                p += 4;
            }
            else // some unrecognized tag
            {
                result.append(p, 1);
                p++;
            }
        }
        else
        {
            result.append(p, 1);
            p++;
        }
    }

    return result;
}


std::wstring GenericError::errorText() const noexcept
{
    switch (code())
    {
    case EUnknown: return std::wstring(L"Unknown error");
    case EOutOfMemory: return std::wstring(L"Out of memory");
    case ETimedOut: return std::wstring(L"Timed out");
    case EIvalidArg: return std::wstring(L"Invalid argument");
    case ENotImplemented: return std::wstring(L"Not implemented");
    case EInvalidRequest: return std::wstring(L"Invalid request");
    }

    return std::wstring();
}

std::wstring getWin32ErrorText(HRESULT r) noexcept
{
    if (r == S_OK)
    {
        return std::wstring();
    }

    return formatMessage(r);
}

std::wstring getNtErrorText(NTSTATUS r) noexcept
{
    return std::wstring(lookupNTSTATUSName(r));
}



} // namespace Util {}

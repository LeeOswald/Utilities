#pragma once


#include <exception>
#include <string>

#include "./Error.hxx"
#include "./Strings.hxx"

namespace Util
{

class Exception
    : public std::exception
{
public:
    ~Exception() noexcept
    {
    }

    Exception() noexcept
    {
    }

    Exception(Error::Ref e) noexcept
        : m_error(e)
    {
    }

    Error::Ref error() const noexcept
    {
        return m_error;
    }

    const char* what() const override
    {
        if (!m_error)
        {
            return "Unspecified exception";
        }

        if (m_cachedText.empty())
        {
            m_cachedText = ws2s(CP_THREAD_ACP, m_error->errorText());
        }

        return m_cachedText.c_str();
    }

private:
    Error::Ref m_error;
    mutable std::string m_cachedText;
};

} // namespace Util {}

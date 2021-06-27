#include "../../Util/IntrusiveList.hxx"
#include "../../Core/Trace.hxx"

#include <crtdbg.h>

struct Doll
    : public Util::IntrusiveList<Doll>::Node
{

    ~Doll()
    {
        wchar_t tmp[256];
        ::_snwprintf_s(tmp, _countof(tmp), L"~Doll() %d\n", i);
        ::OutputDebugStringW(tmp);
    }

    Doll(int i)
        : i(i)
    {
        wchar_t tmp[256];
        ::_snwprintf_s(tmp, _countof(tmp), L"Doll() %d\n", i);
        ::OutputDebugStringW(tmp);
    }

    int i = 0;
};


int main()
{
    ::SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);

#ifdef _DEBUG
    int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    tmpFlag |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(tmpFlag);
#endif

    Core::CurrentThread::setName("Main");

    Core::Trace::initialize(true);

    Core::Trace::writeDebug("Main", __FILE__, __LINE__, L"Fuck you %d times", 8);

    Core::Trace::finaliize();

    {
        Util::IntrusiveList<Doll> l;
        l.push_back(new Doll(1));
        l.push_back(new Doll(2));
        l.push_back(new Doll(3));
        l.push_back(new Doll(4));
        l.push_back(new Doll(5));

        for (auto const& i : l)
        {
            wchar_t tmp[256];
            ::_snwprintf_s(tmp, _countof(tmp), L"present %d\n", i.i);
            ::OutputDebugStringW(tmp);
        }
    }

    return 0;
}


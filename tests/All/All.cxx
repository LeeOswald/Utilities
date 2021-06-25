#include "../../Trace.hxx"


int main()
{
    Util::Trace::initialize(true);

    Util::Trace::writeDebug("Main", __FILE__, __LINE__, L"Fuck you %d times", 8);

    Util::Trace::finaliize();


    return 0;
}


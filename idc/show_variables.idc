
#include <idc.idc>

static main()
{
    auto x, BytePtr;
    for ( x = 1; x <= GetEntryPointQty(); x = x + 1) {
        BytePtr = GetEntryPoint(GetEtryOrdinal(x));
    }



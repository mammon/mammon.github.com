
//-------------------------------------------------------------------------------------------------------
//Res.idc: Parse .rsrc segment directory, displays resources within IDA code display
//Requires library file Reslib.idc to function
//
// code by mammon_ All rights reversed
//-------------------------------------------------------------------------------------------------------

#define _RECURSE_               1
#define _DISPLAY_RESOURCES_     1

#include <idc.idc>
#include <reslib.idc>

static main(){
    auto x, ea;
    ea = GetResDir();
    Message("Parsing Resource Directory [WARNING: This may take 5-10 minutes per MB of file size] ...\n");
    ParseRootHdr(ea);
    Message("Resource parsing completed.\n");
}



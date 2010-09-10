
// This IDC script will alter the names of imported functions to include the
// repeatable comments for that function, such that the name KERNEL_91 in IDA
// becomes KERNEL_91_INITTASK . Run from the Extern segment.
// extrn_name.idc
// code by mammon_ rev 1.27
#include <idc.idc>

static main(){
    auto ea, old_name, old_cmt;
    ea = SegStart( ScreenEA() );
    while ( ea != BADADDR ) {
        old_name = Name( ea );
        old_cmt = RptCmt( ea );
        MakeName( ea, old_name + "_" + substr(old_cmt, 0, (strlen(old_cmt)-1) ) );
        ea = NextAddr(ea);
    }
    Message("Done fixing imported function names.\n");
}



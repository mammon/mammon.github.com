
//seg_string.idc
//This IDC script will parse the strings in a segment and enter the names as
//repeatable comments. The script will default to the segment that the cursor
// is currently located in.
//code by mammon_ rev 1.27

#include <idc.idc>

static main(){
    auto ea;
//    ea = AskSeg( SegStart( ScreenEA() ), "Choose a segment" );
    ea = SegStart( ScreenEA() );
    while ( ea < SegEnd(ea)) {
        if ( Byte(ea) > 0x19 && Byte(ea) < 0x7F){
            MakeStr( ea, -1 );
            MakeRptCmt(ea, Name(ea));
            ea = ea + ItemSize( ea );
        }
        else ea = ea + 1;
    }
    Message("Done with string parsing");
}



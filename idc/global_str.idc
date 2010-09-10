

//Global_str.idc
// This script is similar to seg_string.idc and is a complement to ss.idc. The purpose of
// this script is to parse strings globally; that is, to traverse each segment of the executable
// and test each unexplored byte to determine if it is an ASCII character. If so, the byte is
// converted into a string according to the default string type (for Win32 this should be
// Unicode, for Win16 and dos this should be C. Be sure to increase the maximum length of
// ascii names to 120 characters if you plan to use this script with ss.idc.
//
//code by mammon_ rev 10.17 (bug-report gratis Tomas Strelkauskas)

#include <idc.idc>

static main(){
    auto x, ea, end_seg, oldea;
    ea = FirstSeg();
    end_seg = SegEnd(ea);
    while ( ea != BADADDR) {
        ea = FindUnexplored  ( ea, 1 );
        if ( Byte(ea) > 0x19 && Byte(ea) < 0x7F){
            MakeStr( ea, -1 );
            MakeRptCmt(ea, Name(ea));
            ea = ea + ItemSize( ea );
        }
        if (ea >= end_seg){
                oldea = ea - 1;
                ea = NextSeg(ea);
                if (ea!=BADADDR) end_seg = SegEnd(ea);
                else Message("\nFinal Segment Parsed: " + SegName( SegStart(oldea) ) + "\n");
        }
    }
    Message("Done with string parsing.\n");
}






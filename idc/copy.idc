
//------------------------------------------------------------------------------------------------------
//copy.idc (rev 3.09): Outputs selected text to an .asm file
//Usage: Select text with mouse or cursor, hit F2 and type copy.idc, enter a filename when prompted
//       and the selected text will be written to that file.
//Future Plans: Make this output to the Windows clipboard. I may have to patch IDA for this....
//
// code by mammon_ All rights reversed, use as you see fit.....
//------------------------------------------------------------------------------------------------------
#include <idc.idc>

static main(){
    auto filename, start_loc, end_loc;
    start_loc = SelStart();
    end_loc = SelEnd();
    filename = AskFile( "asm", "Output file name?");
    WriteTxt( filename, start_loc, end_loc);
    return 0;
}



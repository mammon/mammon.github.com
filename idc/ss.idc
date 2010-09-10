
//ss.idc
// This is intended to mimic the functionality of ss.exe (string search) by outputting all
// defined strings to the IDA message window (and, optionally, to an output file). Note that
// a string must be defined (with the name starting with "a", such as "aUnregisteredDemo") in
// order for ss.idc to catch it, so you should run through the various data segments with
// segstrings.idc to create the strings. I may produce a global strings script to automate
// this task as well.
//
// String reading is not handled well in IDA; this script uses the name of the string address
// to represent the string in its output. Therefore, it is a good idea to put the following line
// in your IGA.CFG:
//      MAX_NAMES_LENGTH  = 120
// This sets address names to a max of 120 characters, so that a good portion of the string will
// be represented in the output of ss.idc
//
//code by mammon_ rev 4.02

#include <idc.idc>

static main(){
    auto x, ea, end_seg, write, OutFName, Outfile;
    write = AskYN( 0, "Write output to file?");
    if ( write == 1){
        OutFName = AskFile( "*.txt", "Name of output file?");
        Outfile = fopen(OutFName, "wt") ;
    }
    ea = FirstSeg();
    end_seg = SegEnd(ea);
    while ( ea != BADADDR) {
        if( substr( Name(ea), 0, 1) == "a") {
            Message( "String " + Name(ea) +" found at " + atoa(ea) + "\n" );
            if ( write == 1){
                fprintf(Outfile, "String " + Name(ea) +" found at " + atoa(ea) + "\n" );
            }
        }
        if (ea == end_seg){
                ea = NextSeg(ea);
                if (ea!=BADADDR) end_seg = SegEnd(ea);
        }
        else ea = NextHead(ea);
    }
    if ( write == 1){
        fclose (Outfile);
    }
    Message( "String Search complete!\n\n" );
}



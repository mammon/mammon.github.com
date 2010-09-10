
// Entrytree.idc code per mammon_ ("use, steal, change, but do not claim.")
// Outputs to the message window a recursive tree of the calls from each entry point of the file

#include <idc.idc>

static OutputLine(ea, x, nest){
    auto j;
    for ( j = 0; j <= nest; j = j + 1) {
        Message("   ");
    }
    Message(atoa(ea) + " refers to " + Name(x) + " : " + atoa(x) + "\n");
}
static GetXrefs(ea, nest){
    auto x;
    nest = nest + 1;
    x = Rfirst0(ea);
    if ( x != BADADDR) {
    OutputLine(ea, x, nest);
    GetXrefs(x, nest);
    x = Rnext0(ea,x);
    }
    while ( x != BADADDR) {
        OutputLine(ea, x, nest);
        GetXrefs(x, nest);
        x = Rnext0(ea,x);
    }
    nest = nest - 1;
}

static main(){
    auto ea, x, i, nest, f_end, EPOrd;
    nest = -1;
    Message("\n*** Code References from " + GetFunctionName(ea) + " : "  + atoa(ea) + "\n");
    for ( i = 0; i < GetEntryPointQty(); i = i + 1) {
        Message("Entry Point " + ltoa(i, 10) + "\n");
        EPOrd = GetEntryOrdinal(i);
        ea = GetEntryPoint(EPOrd);
        f_end = FindFuncEnd(ea);
        for ( ea ; ea <= f_end; ea = NextAddr(ea) ) {
            GetXrefs(ea, nest);
        }
    }
    Message("End of output. \n");
}



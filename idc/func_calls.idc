
#include <idc.idc>

static main(){
    auto ea,x,f_end;
    ea = ChooseFunction("Select a function to parse:");
    f_end = FindFuncEnd(ea);
    Message("\n*** Code References from " + GetFunctionName(ea) + " : "  + atoa(ea) + "\n");
    for ( ea ; ea <= f_end; ea = NextAddr(ea) ) {
        x = Rfirst0(ea);
        if ( x != BADADDR) {
        Message(atoa(ea) + " refers to " + Name(x) + " : " + atoa(x) + "\n");
        x = Rnext0(ea,x);
        }
        while ( x != BADADDR) {
            Message(atoa(ea) + " refers to " + Name(x) + " : " + atoa(x) + "\n");
            x = Rnext0(ea,x);
        }
    }
    Message("End of output. \n");
}



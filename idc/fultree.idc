
// by mammon_ / the_owl / slava

#include <idc.idc>

static OutputLine(ea, x, nest, OutFileH){
    auto j;
    for ( j = 0; j <= nest; j = j + 1) {
        fprintf(OutFileH, "   ");
    }
    fprintf(OutFileH, atoa(ea) + "  ->  " + Name(x) + " : " + atoa(x) + "\n");
}

static GetXrefs(ea, nest, OutFileH){
    auto x, f_end;
    x = Rfirst0(ea);
    if (x != BADADDR) {
        OutputLine(ea, x, nest, OutFileH);
    }
}

static TravelFunc(ea, nest, MaxNest , ArrayID, OutFileH) {
   auto x, f_end, type, counter;
   if (nest >= xtol(MaxNest)-1)
      return;

   f_end = FindFuncEnd(ea);  // note that FindFuncEnd returns the address of (FunctionEnd+1)
   for (ea; ea < f_end; ea=NextAddr(ea)) {
      x = Rfirst0(ea);
      if (x != BADADDR) {
         type = XrefType();
         if (type == fl_CF || type == fl_CN) {

            for(counter = 0; counter <= GetLastIndex(AR_STR,ArrayID); counter = counter+1){ //go through array	
		if (Name(x) == GetArrayElement(AR_STR,ArrayID,counter)){  // do we have this name stored in array?
			OutputLine(ea, x, nest+1, OutFileH);  //out fuction call
			fprintf(OutFileH, "***** RECURSIVE CALL OF FUNCTION   %s *****\n", Name(x)); //out warning
			DelArrayElement(AR_STR,ArrayID,nest+1); // going up, so let's adjust our array.
			return; // hmm.. we may still need some space on our h/d, so let's get out of here right now...
		}
            }

            OutputLine(ea, x, nest+1, OutFileH);
	    SetArrayString(ArrayID,nest+1,Name(x));  // no such a function in array, so add it
            TravelFunc(x, nest+1, MaxNest, ArrayID, OutFileH); // let's dig deeper
	    DelArrayElement(AR_STR,ArrayID,nest+1); //it's ok to call a func multiple times from the same level
         }
      }
   }
   DelArrayElement(AR_STR,ArrayID,nest+1); // going up, so let's adjust our array.
}

static main(){
    auto ea, x, i, nest, f_end, EPOrd, OutFileH, OutFName, MaxNest, ArrayID;

ArrayID =  CreateArray("RecurFuncNames");  //create array for our recursive calls
if (ArrayID == -1){
	Warning("Can't create array RecurFuncNames"); //hmm... 
	ArrayID = GetArrayId("RecurFuncNames");  //let's see if we already have an array with this name
	DeleteArray(ArrayID);			//if it's there, it's most probably because we abnormaly 
						//terminated this script without removing the  array from 
						//the database at the end of main(), so let's try to do it now.
	ArrayID =  CreateArray("RecurFuncNames"); //try to create it again
	if (ArrayID == -1){
		Warning("Still can't create array RecurFuncNames, exiting"); //damn, still doesn't work, so exit
		return;
	}
}

    nest = -1;
	MaxNest = AskStr( "10", "Enter max number of levels");
    OutFName = AskStr("FuncTree.txt", "Enter output filename: ");
    OutFileH = fopen(OutFName, "wt");
        ea = ChooseFunction("select a function to parse");
        f_end = FindFuncEnd(ea);
        fprintf(OutFileH, "*** Code references from " + GetFunctionName(ea) + " : " + atoa(ea) + "\n");
        TravelFunc(ea, nest, MaxNest, ArrayID, OutFileH); // shit.. no globals??. let's pass all this crap then. 
    fclose (OutFileH);
    Message("End of output. \n");
DeleteArray(ArrayID);
}


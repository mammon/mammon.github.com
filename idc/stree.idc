//Stree.idc
//code by cephren (mammon_ / the_owl / slava / modified by cephren)
//
//Documentation:
// stree.idc is derived from fultree.idc that has been published by mammon_ earlier this year.
//
//It produces a call-tree from any function in an IDA disassembly and writes it to a file.
//The main reason why I made it, was, that fultree often makes huge output files, which are
//hard to analyze.
//
//The differences are:
//
//- Functions are traced only once. Further occurences of the same functions are marked
//"Already traced in line# ...." (The line numbers are also new)
//- Optional recursive tracing  only if a functionname starts with the IDA defaults "sub_" 
//or "unknown". If a function is renamed either by IDA when applying signatures or by 
//the user when he has identified the purpose of the function, it will be not traced deeper.
//- Comments like "*** Recursive call.." or "Already Traced..." require no extra line
//- The level of recursive tracing can easily be counted.
//
//Example output line:
//   32   |  |  |  |  |0042B928  ->  sub_42B77E : 0042B77E **** Already Traced in Line 19
//means line 32 in the listing, the 5th tracelevel. At location 0042B928 sub_42B77E is
//called. Sub_42B77E will not be traced deeper for this has already been done at line 19 of
//the listing.


#include <idc.idc>

static OutputLine(lnumber, ea, x, nest, OutFileH){
    auto j;
    fprintf(OutFileH, "%5d ", lnumber);
    for ( j = 0; j <= nest; j = j + 1) {
        fprintf(OutFileH, "  |");
    }
    fprintf(OutFileH, atoa(ea) + "  ->  " + Name(x) + " : " + atoa(x));
}


static TravelFunc(ea, nest, MaxNest , ArrayID, ArrayFN, OutFileH, lnumber, UnkOnly) {
   auto x, f_end, type, counter, count2, last, df, df2;
   if (nest >= xtol(MaxNest)-1)
      return (lnumber);

   f_end = FindFuncEnd(ea);  // note that FindFuncEnd returns the address of (FunctionEnd+1)
   for (ea; ea < f_end; ea=NextAddr(ea)) {
      x = Rfirst0(ea);
      if (x != BADADDR) {
         type = XrefType();
         if (type == fl_CF || type == fl_CN) {

            for(counter = 0; counter <= GetLastIndex(AR_STR,ArrayID); counter = counter+1){ //go through array
                if (Name(x) == GetArrayElement(AR_STR,ArrayID,counter)){  // do we have this name stored in array?
                    OutputLine(lnumber, ea, x, nest+1, OutFileH);  //out fuction call
                    lnumber = lnumber + 1;
                    fprintf(OutFileH, "***** RECURSIVE CALL *****\n"); //out warning
                    DelArrayElement(AR_STR,ArrayID,nest+1); // going up, so let's adjust our array.
                    {return lnumber;} // we may still need some space on our h/d, so let's get out of here right now
                }
			}


            OutputLine(lnumber, ea, x, nest+1, OutFileH);
            lnumber = lnumber+1;
            SetArrayString(ArrayID,nest+1,Name(x));  // no such a function in array, so add it
            df2=0;
        	for(count2 = 0; count2 < GetLastIndex(AR_STR,ArrayFN); count2 = count2+2){ //go through AllFunc-array
                if (Name(x) == GetArrayElement(AR_STR,ArrayFN,count2) && (strstr(Name(x), "sub_" )== 0 || strstr(Name(x), "unknown" )== 0 || UnkOnly !=1) && (nest < xtol(MaxNest)-1)){
                	fprintf(OutFileH, " **** Already Traced in Line %d \n", GetArrayElement(AR_LONG,ArrayFN,count2+1));
                	df=1;
                	df2=1;
				}
            }
        	if (df==0){ // add function name and line# to array
            	last = GetLastIndex(AR_STR,ArrayFN);
            	SetArrayString(ArrayFN,last+1,Name(x));
            	SetArrayLong(ArrayFN,last+2,lnumber-1);
            }
        	if (df2==0){ // no additional comments, so print LF
            	fprintf(OutFileH, "\n");
            }
            if (((strstr(Name(x), "sub_" )== 0 || strstr(Name(x), "unknown" )== 0) && (df == 0))|| UnkOnly !=1){ // trace only functions starting with "sub_" & not traced before 
                lnumber = TravelFunc(x, nest+1, MaxNest, ArrayID, ArrayFN, OutFileH, lnumber, UnkOnly); // let's dig deeper
            }
            DelArrayElement(AR_STR,ArrayID,nest+1); //it's ok to call a func multiple times from the same level
         }
      }
   }
   DelArrayElement(AR_STR,ArrayID,nest+1); // going up, so let's adjust our array for recursive calls.
{return lnumber;}
}

static main(){
    auto ea, x, i, nest, f_end, EPOrd, OutFileH, OutFName, MaxNest, ArrayID, ArrayFN, lnumber, UnkOnly;

	ArrayID =  CreateArray("RecurFuncNames");  //create array for our recursive calls
	if (ArrayID == -1){
        Warning("Can't create array RecurFuncNames"); //hmm...
        ArrayID = GetArrayId("RecurFuncNames");  //let's see if we already have an array with this name
        DeleteArray(ArrayID);                   //if it's there, it's most probably because we abnormaly
                                                //terminated this script without removing the  array from
                                                //the database at the end of main(), so let's try to do it now.
        ArrayID =  CreateArray("RecurFuncNames"); //try to create it again
        if (ArrayID == -1){
        	Warning("Still can't create array RecurFuncNames, exiting"); // still doesn't work, so exit
        	return;
        }
	}
	ArrayFN =  CreateArray("AllFuncNames");  //create array for function names and line# of first occurence
	if (ArrayFN == -1){
        Warning("Can't create array AllFuncNames"); // Comments see above
        ArrayFN = GetArrayId("AllFuncNames");
        DeleteArray(ArrayFN);

        ArrayFN =  CreateArray("AllFuncNames");
        if (ArrayFN == -1){
                Warning("Still can't create array AllFuncNames, exiting"); // still doesn't work, so exit
                return;
        }
	}
    nest = -1;
    MaxNest = AskStr( "10", "Enter max number of levels");
	UnkOnly = AskYN(1, "Trace sub.. and unknown.. funktions only?");
    OutFName = AskStr("FuncTree.txt", "Enter output filename: ");
    OutFileH = fopen(OutFName, "wt");
    ea = ChooseFunction("select a function to parse");
    f_end = FindFuncEnd(ea);
    fprintf(OutFileH, "*** Code references from " + GetFunctionName(ea) + " : " + atoa(ea) + "\n");
    lnumber = 1;
    lnumber = TravelFunc(ea, nest, MaxNest, ArrayID, ArrayFN, OutFileH, lnumber, UnkOnly); // shit.. no globals??. let's pass all this crap then.
    fclose (OutFileH);
    Message("End of output. \n");
	DeleteArray(ArrayID);
	DeleteArray(ArrayFN);
}


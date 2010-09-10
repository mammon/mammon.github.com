
//func_tree.idc
//This script will prompt the user for a function name, then will output all of the calls
//made by that function...it will travel recursively into each call, up to 25 "levels" deep,
//to determine what calls are made by each routine called by the main function.
//This script is part of an attempt to give IDA some code-abstracton functionality. Its
//main purpose is to show what subroutines and API routines are called by a user-determined
//function. The output is printed in the IDA message window.


#include <idc.idc>    //provides IDC function prototypes

static OutputXref(ea, x, nest)   //Prints Calls to the Message Window
{
    auto j;
    for ( j; j <= nest; j = j + 1) {   //nesting routine: indents two spaces for every
        Message("  ");                 //level of recursion
    }
    Message( atoa(ea) + " refers to " + Name(x) + "\n");  //outputs a code address and what routine
}                                                         //that address calls

static GetXrefs(ea, nest)    //recursively travels though the program, outputting each
{                            //call it finds via OutputXref()
    auto x, t, f_end;
    nest = nest + 1;         //determine level of indentation for the output of calls
    f_end = FindFuncEnd(ea); //made by this function
    for ( ea; ea <= f_end; ea = NextAddr(ea) ) { //For each Address in this function/routine
        x = Rfirst0(ea);                        //Get Xref for this address
        if ( x != BADADDR && nest < 25 ) {      //While there is an Xref
            t = XrefType();                      //Get what type of Xref this is (jmp, call, etc)
            if ( (t == fl_CF) || (t == fl_CN)) { //Output & recurse only if a call
                OutputXref (ea, x, nest);
                GetXrefs(x, nest);  //call GetXrefs to get that calls made by this called routine
            }                       //(Recursion)
        }
    }
    nest = nest - 1;                //undo indentation before exiting this function/routine
}

static main()
{
    auto ea, nest;
    nest = 0;
    ea = ChooseFunction("Select a function to parse:");  //ask user for function name
    GetXrefs(ea, nest);                                  //Bulk of program
    Message("End of output.\n");
}




//------------------------------------------------------------------------------------------------------
//Strings.idc (rev 1.30): Parses PE file String Table
//Note: This script demonstrates how to parse a single resource using Reslib.idc.
//Resource Type may be configured by changing two variables within the script
//Requires: Reslin.idc
//
// code by mammon_ All rights reversed, use as you see fit.....
//------------------------------------------------------------------------------------------------------


#define _RECURSE_               1        //Recurse Directories in header
#define _DISPLAY_RESOURCES_     1        //Display Resources

#include <idc.idc>
#include <Reslib.idc>


static main(){
    auto ea, num_entries, x, res_type, res_type_id;
    res_type = "String_Table";
    //res_type can be configured according to the table at the end of the file
    res_type_id = 0x06;
    //res_id can be configured according to the table at the end of the file
    ea = GetResDir();
    num_entries = Word(ea + 12) + Word (ea + 14);  //Get number of entries in .rsrc Header
    ea = ea +16;
    for ( x = 0; x <= num_entries; x = x + 1 ) {   //For each entry
        if (Dword(ea) == res_type_id){             //check if it is the right resource
            ParseSubDirHdr(ResolveOffset(ea + 4), res_type, 0, 0 );  //if so, parse
        }
        ea = ea + 8;
    }
    Message( res_type + " Parsing finished.\n");
}

// Possible Resource Types:
//    Res_ID == 0x01 Resource_Type = "Cursor";
//    Res_ID == 0x02 Resource_Type = "Bitmap";
//    Res_ID == 0x03 Resource_Type = "Icon";
//    Res_ID == 0x04 Resource_Type = "Menu";
//    Res_ID == 0x05 Resource_Type = "Dialog";
//    Res_ID == 0x06 Resource_Type = "String_Table";
//    Res_ID == 0x07 Resource_Type = "Font_Directory";
//    Res_ID == 0x08 Resource_Type = "Font";
//    Res_ID == 0x09 Resource_Type = "Accelerator_Table";
//    Res_ID == 0x0A Resource_Type = "RCDATA_(Dynamically_Defined_Resource)";
//    Res_ID == 0x0B Resource_Type = "Message_Table";
//    Res_ID == 0x0C Resource_Type = "Group_Cursor";
//    Res_ID == 0x0E Resource_Type = "Group_Icon";
//    Res_ID == 0x10 Resource_Type = "Version";
//    Res_ID == 0x11 Resource_Type = "DLG_Include";
//    Res_ID == 0x13 Resource_Type = "PlugPlay";
//    Res_ID == 0x14 Resource_Type = "VXD";
//    Res_ID == 0x15 Resource_Type = "AniCursor";
//    Res_ID == 0x2002 Resource_Type = "NewBitmap";
//    Res_ID == 0x2004 Resource_Type = "NewMenu";
//    Res_ID == 0x2005 Resource_Type = "NewDialog";




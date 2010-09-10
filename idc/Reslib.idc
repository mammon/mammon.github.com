
//------------------------------------------------------------------------------------------------------
//Reslib.idc (rev 1.31): Resource parsing/handling functions for the PE file format
//                       _____Function_____Listing_____
//GetResDir(), ResolveOffset( ea ), ParseResDirOld( ea ), MakeDword_Cmt( ea, char ),
//MakeWord_Cmt( ea, char), IDResource( long), ParseUnkRes( ea, ea),
//ParseLeafNode( ea, char, long ), ParseSubDirData( ea, char, long, long ),  ParseDirHdr( ea ),
//ParseSubDirHdr( ea, char, long, long ), ParseRootData( ea ), ParseRootHdr( ea)
//
// Thanks to Quine and Ssspyder for help with the enums implementation (file under "eternal gratitude" ;)
// code by mammon_ All rights reversed, use as you see fit.....
//------------------------------------------------------------------------------------------------------

#include <idc.idc>

#ifndef _RECURSE_
#define _RECURSE_               0
#endif

#ifndef _DISPLAY_RESOURCES_
#define _DISPLAY_RESOUCRES_     0
#endif

//GetResourceDirectory checks each segment of the program until it finds one named .rsrc
//Syntax: GetResDir()
//Returns: ea starting address of .rsrc segment
static GetResDir()
{
    auto ea, next, name;
    ea = FirstSeg();
    next = ea;
    while ( (next = NextSeg(next)) != -1) {
        name = SegName(next);
        if ( substr( name, 0, 5 ) == ".rsrc" ) break;
   }
   return next;
}

//ResolveOffset adds the offset of a res data/subdir to the base address of the .rsrc directory
//The offset is stored in the program as data; this data is read and added to the .rsrc RVA
//Syntax: ResolveOffset(ea address_containing_offset_to_be_resolved )
//Returns: ea complete address
static ResolveOffset( BytePtr )
{
    auto res_offset;
    res_offset = SegStart(BytePtr) + Word(BytePtr);
    return res_offset;
}

static StripZeros( BytePtr )
{
    if( Byte(BytePtr + 1) == 0 ) {
        if( Byte(BytePtr + 2) == 0 && Byte(BytePtr + 3) == 0) {
            MakeDword(BytePtr);
            BytePtr = BytePtr + 4;
        }
        else {
            MakeWord( BytePtr );
            BytePtr = BytePtr + 2;
        }
    }
    else {
        MakeByte( BytePtr );
        BytePtr = BytePtr + 1;
    }
    return BytePtr;
}

//ParseResDirOld takes no parameters, and creates two databases containing the elementary
//structure of the resource tree as given in the .rsrc directory header
//Note: Not currently functional; included for salvage purposes
//Syntax: ParseResDirOld(ea starting_address_of_.rsrc_directory)
static ParseResDirOld(ea)
{
    auto aResType, aResLoc, BytePtr, x, total_entries;
    aResType = CreateArray( "RES_TYPE" );
    aResLoc = CreateArray( "RES_LOC" );
    BytePtr = SegStart( ea ) + 12;
    total_entries = Word( BytePtr ) + Word( BytePtr + 2 );
    BytePtr = BytePtr + 4;
    for ( x = 1; x <= total_entries; x = x + 1 ) {
        SetArrayString( aResType, x, IdentifyTypeID( BytePtr ) );
        BytePtr = BytePtr + 4;
        SetArrayLong( aResLoc, x, ResolveOffset( BytePtr ) );
        BytePtr = BytePtr + 4;
    }
}

//MakeDoubleWordWithComment: Defines 4 Unexplored bytes as a DWord (data), adds a posterior comment line,
//and increments BytePtr by 4
//Syntax: MakeDWord_Cmt(ea address_of_leading_byte, char comment_line)
//Returns: ea BytePtr
static MakeDword_Cmt(BytePtr, ResComment)
{
    MakeDword(BytePtr);
    //ExtLinB(BytePtr, 0, ResComment);
    MakeComm(BytePtr, ResComment);
    BytePtr = BytePtr + 4;
    return BytePtr;
}

//MakeWordWithComment: Defines 2 Unexplored bytes as a Word (data), adds a posterior comment line,
//and increments BytePtr by 2
//Syntax: MakeWord_Cmt(ea address_of_leading_byte, char comment_line)
//Returns: ea BytePtr
static MakeWord_Cmt(BytePtr, ResComment)
{
    MakeWord(BytePtr);
    //ExtLinB(BytePtr, 0, ResComment);
    MakeComm(BytePtr, ResComment);
    BytePtr = BytePtr + 2;
    return BytePtr;
}

//MakeByteWithComment: Defines 1 Unexplored byte as a Byte (data), adds a posterior comment line,
//and increments BytePtr by 1
//Syntax: MakeByte_Cmt(ea address_of_byte, char comment_line)
//Returns: ea BytePtr
static MakeByte_Cmt(BytePtr, ResComment)
{
    MakeByte(BytePtr);
    //ExtLinB(BytePtr, 0, ResComment);
    MakeComm(BytePtr, ResComment);
    BytePtr = BytePtr + 1;
    return BytePtr;
}

//IdentifyResource: Translates a resource TypeID to its character name/designation
//Syntax: ID_Resource(long TypeID)
//Returns: char Resource_Type
static ID_Resource(Res_ID)
{
    auto Resource_Type;
    if ( Res_ID == 0x01) Resource_Type = "Cursor";
    else if (Res_ID == 0x02) Resource_Type = "Bitmap";
    else if (Res_ID == 0x03) Resource_Type = "Icon";
    else if (Res_ID == 0x04) Resource_Type = "Menu";
    else if (Res_ID == 0x05) Resource_Type = "Dialog";
    else if (Res_ID == 0x06) Resource_Type = "String_Table";
    else if (Res_ID == 0x07) Resource_Type = "Font_Directory";
    else if (Res_ID == 0x08) Resource_Type = "Font";
    else if (Res_ID == 0x09) Resource_Type = "Accelerator_Table";
    else if (Res_ID == 0x0A) Resource_Type = "RCDATA_(Dynamically_Defined_Resource)";
    else if (Res_ID == 0x0B) Resource_Type = "Message_Table";
    else if (Res_ID == 0x0C) Resource_Type = "Group_Cursor";
    else if (Res_ID == 0x0E) Resource_Type = "Group_Icon";
    else if (Res_ID == 0x10) Resource_Type = "Version";
    else if (Res_ID == 0x11) Resource_Type = "DLG_Include";
    else if (Res_ID == 0x13) Resource_Type = "PlugPlay";
    else if (Res_ID == 0x14) Resource_Type = "VXD";
    else if (Res_ID == 0x15) Resource_Type = "AniCursor";
    else if (Res_ID == 0x2002) Resource_Type = "NewBitmap";
    else if (Res_ID == 0x2004) Resource_Type = "NewMenu";
    else if (Res_ID == 0x2005) Resource_Type = "NewDialog";
    else Resource_Type = "Unknown_Resource_Type " + ltoa(Res_ID, 10);
    return Resource_Type;
}

//MakeDataOffset determines the RVA of a data offset (distance of offset from start of file, not seg)
//Syntax: MakeDataOffset( ea address_containing_offset_to_data )
//Returns: ea RVA (Relative Virtual Address) of data
static MakeDataOffset(BytePtr)
{
    auto Data_Offset;
    Data_Offset = (FirstSeg() - 0x1000) + Dword(BytePtr);
    return Data_Offset;
}

//ParseStrTable: Formats String Table
//Syntax: ParseStringTable(ea address_of_resource_to_parse, ea address_of_next_resource)
static ParseStrTable(BytePtr, Next_Resource, str_tbl_num)
{
        auto curr_enum, str_num, str_rc_id, enum_id;
        SetLongPrm(INF_STRTYPE, ASCSTR_UNICODE);
        curr_enum = GetEnumQty() + 1;
        str_num = 0;
        enum_id = AddEnum(curr_enum, "String_Table" + ltoa(BytePtr, 16), FF_0NUMH);
        while ( BytePtr < Next_Resource) {
            if ( Word(BytePtr) != 0x00) {
                str_rc_id = ( str_tbl_num - 1) * 16 + str_num;
                MakeWord(BytePtr);
                MakeStr(BytePtr + 2, BytePtr + (2 * (Byte(BytePtr) + 1)));
                AddConst(enum_id, "string" + ltoa(BytePtr, 16), str_rc_id);
                SetConstCmt( GetConst(enum_id, str_rc_id), Name(BytePtr + 2), 1);
                str_num = str_num + 1;
                BytePtr = BytePtr + ( 2 * Byte(BytePtr)) + 2;
            }
            else if ( Dword(BytePtr) == 0x0000 && (BytePtr + 6) > Next_Resource) {
                MakeDword(BytePtr);
                break;
                }
            else {
                MakeWord(BytePtr);
                str_num = str_num + 1;
                BytePtr = BytePtr + 2;
            }
        }
}

//IdentifyVirtualKeyCode
//Syntax: IDVKeyCode ( long virtual_key_code )
static IDVKeyCode(VKeyCode)
{
    auto KeyCmd;
    KeyCmd="";
    //Note: The following is awkward as IDA does not allow Switch statements or a large number of
    //else if statements...however it allows infinite "if" statements
    if (VKeyCode == 0x01) KeyCmd = "VK_LBUTTON";
    if (VKeyCode == 0x02  ) KeyCmd = "VK_RBUTTON";
    if (VKeyCode == 0x03  ) KeyCmd = "VK_CANCEL";
    if (VKeyCode == 0x04  ) KeyCmd = "VK_MBUTTON";
    if (VKeyCode == 0x08  ) KeyCmd = "VK_BACK";
    if (VKeyCode == 0x09  ) KeyCmd = "VK_TAB";
    if (VKeyCode == 0x0C  ) KeyCmd = "VK_CLEAR";
    if (VKeyCode == 0x0D  ) KeyCmd = "VK_RETURN";
    if (VKeyCode == 0x10  ) KeyCmd = "VK_SHIFT";
    if (VKeyCode == 0x11  ) KeyCmd = "VK_CONTROL";
    if (VKeyCode == 0x12  ) KeyCmd = "VK_MENU(ALT)";
    if (VKeyCode == 0x13  ) KeyCmd = "VK_PAUSE";
    if (VKeyCode == 0x14  ) KeyCmd = "VK_CAPITAL";
    if (VKeyCode == 0x1B  ) KeyCmd = "VK_ESCAPE";
    if (VKeyCode == 0x20  ) KeyCmd = "VK_SPACE";
    if (VKeyCode == 0x21  ) KeyCmd = "VK_PRIOR";
    if (VKeyCode == 0x22  ) KeyCmd = "VK_NEXT";
    if (VKeyCode == 0x23  ) KeyCmd = "VK_END";
    if (VKeyCode == 0x24  ) KeyCmd = "VK_HOME";
    if (VKeyCode == 0x25  ) KeyCmd = "VK_LEFT";
    if (VKeyCode == 0x26  ) KeyCmd = "VK_UP";
    if (VKeyCode == 0x27  ) KeyCmd = "VK_RIGHT";
    if (VKeyCode == 0x28  ) KeyCmd = "VK_DOWN";
    if (VKeyCode == 0x29  ) KeyCmd = "VK_SELECT";
    if (VKeyCode == 0x2A  ) KeyCmd = "VK_PRINT";
    if (VKeyCode == 0x2B  ) KeyCmd = "VK_EXECUTE";
    if (VKeyCode == 0x2C  ) KeyCmd = "VK_SNAPSHOT";
    if (VKeyCode == 0x2D  ) KeyCmd = "VK_INSERT";
    if (VKeyCode == 0x2E  ) KeyCmd = "VK_DELETE";
    if (VKeyCode == 0x2F  ) KeyCmd = "VK_HELP";
    if (VKeyCode == 0x30  ) KeyCmd = "VK_0";
    if (VKeyCode == 0x31  ) KeyCmd = "VK_1";
    if (VKeyCode == 0x32  ) KeyCmd = "VK_2";
    if (VKeyCode == 0x33  ) KeyCmd = "VK_3";
    if (VKeyCode == 0x34  ) KeyCmd = "VK_4";
    if (VKeyCode == 0x35  ) KeyCmd = "VK_5";
    if (VKeyCode == 0x36  ) KeyCmd = "VK_6";
    if (VKeyCode == 0x37  ) KeyCmd = "VK_7";
    if (VKeyCode == 0x38  ) KeyCmd = "VK_8";
    if (VKeyCode == 0x39  ) KeyCmd = "VK_9";
    if (VKeyCode == 0x41  ) KeyCmd = "VK_A";
    if (VKeyCode == 0x42  ) KeyCmd = "VK_B";
    if (VKeyCode == 0x43  ) KeyCmd = "VK_C";
    if (VKeyCode == 0x44  ) KeyCmd = "VK_D";
    if (VKeyCode == 0x45  ) KeyCmd = "VK_E";
    if (VKeyCode == 0x46  ) KeyCmd = "VK_F";
    if (VKeyCode == 0x47  ) KeyCmd = "VK_G";
    if (VKeyCode == 0x48  ) KeyCmd = "VK_H";
    if (VKeyCode == 0x49  ) KeyCmd = "VK_I";
    if (VKeyCode == 0x4A  ) KeyCmd = "VK_J";
    if (VKeyCode == 0x4B  ) KeyCmd = "VK_K";
    if (VKeyCode == 0x4C  ) KeyCmd = "VK_L";
    if (VKeyCode == 0x4D  ) KeyCmd = "VK_M";
    if (VKeyCode == 0x4E  ) KeyCmd = "VK_N";
    if (VKeyCode == 0x4F  ) KeyCmd = "VK_O";
    if (VKeyCode == 0x50  ) KeyCmd = "VK_P";
    if (VKeyCode == 0x51  ) KeyCmd = "VK_Q";
    if (VKeyCode == 0x52  ) KeyCmd = "VK_R";
    if (VKeyCode == 0x53  ) KeyCmd = "VK_S";
    if (VKeyCode == 0x54  ) KeyCmd = "VK_T";
    if (VKeyCode == 0x55  ) KeyCmd = "VK_U";
    if (VKeyCode == 0x56  ) KeyCmd = "VK_V";
    if (VKeyCode == 0x57  ) KeyCmd = "VK_W";
    if (VKeyCode == 0x58  ) KeyCmd = "VK_X";
    if (VKeyCode == 0x59  ) KeyCmd = "VK_Y";
    if (VKeyCode == 0x5A  ) KeyCmd = "VK_Z";
    if (VKeyCode == 0x5B  ) KeyCmd = "VK_LWIN";
    if (VKeyCode == 0x5C  ) KeyCmd = "VK_RWIN";
    if (VKeyCode == 0x5D  ) KeyCmd = "VK_APS";
    if (VKeyCode == 0x60  ) KeyCmd = "VK_NUMPAD0";
    if (VKeyCode == 0x61  ) KeyCmd = "VK_NUMPAD1";
    if (VKeyCode == 0x62  ) KeyCmd = "VK_NUMPAD2";
    if (VKeyCode == 0x63  ) KeyCmd = "VK_NUMPAD3";
    if (VKeyCode == 0x64  ) KeyCmd = "VK_NUMPAD4";
    if (VKeyCode == 0x65  ) KeyCmd = "VK_NUMPAD5";
    if (VKeyCode == 0x66  ) KeyCmd = "VK_NUMPAD6";
    if (VKeyCode == 0x67  ) KeyCmd = "VK_NUMPAD7";
    if (VKeyCode == 0x68  ) KeyCmd = "VK_NUMPAD8";
    if (VKeyCode == 0x69  ) KeyCmd = "VK_NUMPAD9";
    if (VKeyCode == 0x6A  ) KeyCmd = "VK_MULTIPLY";
    if (VKeyCode == 0x6B  ) KeyCmd = "VK_ADD";
    if (VKeyCode == 0x6C  ) KeyCmd = "VK_SEPERATOR";
    if (VKeyCode == 0x6D  ) KeyCmd = "VK_SUBTRACT";
    if (VKeyCode == 0x6E  ) KeyCmd = "VK_DECIMAL";
    if (VKeyCode == 0x6F  ) KeyCmd = "VK_DIVIDE";
    if (VKeyCode == 0x70  ) KeyCmd = "VK_F1";
    if (VKeyCode == 0x71  ) KeyCmd = "VK_F2";
    if (VKeyCode == 0x72  ) KeyCmd = "VK_F3";
    if (VKeyCode == 0x73  ) KeyCmd = "VK_F4";
    if (VKeyCode == 0x74  ) KeyCmd = "VK_F5";
    if (VKeyCode == 0x75  ) KeyCmd = "VK_F6";
    if (VKeyCode == 0x76  ) KeyCmd = "VK_F7";
    if (VKeyCode == 0x77  ) KeyCmd = "VK_F8";
    if (VKeyCode == 0x78  ) KeyCmd = "VK_F9";
    if (VKeyCode == 0x79  ) KeyCmd = "VK_F10";
    if (VKeyCode == 0x7A  ) KeyCmd = "VK_F11";
    if (VKeyCode == 0x7B  ) KeyCmd = "VK_F12";
    if (VKeyCode == 0x90  ) KeyCmd = "VK_NUMLOCK";
    if (VKeyCode == 0x91  ) KeyCmd = "VK_SCROLL";
    if (VKeyCode == 0xA0  ) KeyCmd = "VK_LSHIFT";
    if (VKeyCode == 0xA1  ) KeyCmd = "VK_RSHIFT";
    if (VKeyCode == 0xA2  ) KeyCmd = "VK_LCONTROL";
    if (VKeyCode == 0xA3  ) KeyCmd = "VK_RCONTROL";
    if (VKeyCode == 0xA4  ) KeyCmd = "VK_LMENU(ALT)";
    if (VKeyCode == 0xA5  ) KeyCmd = "VK_RMENU(ALT)";
    if (VKeyCode == 0xE5  ) KeyCmd = "VK_PROCESSKEY";
    if (VKeyCode == 0xF6  ) KeyCmd = "VK_ATTN";
    if (VKeyCode == 0xF7  ) KeyCmd = "VK_CRSEL";
    if (VKeyCode == 0xF8  ) KeyCmd = "VK_EXSEL";
    if (VKeyCode == 0xF9  ) KeyCmd = "VK_EREOF";
    if (VKeyCode == 0xFA  ) KeyCmd = "VK_PLAY";
    if (VKeyCode == 0xFB  ) KeyCmd = "VK_ZOOM";
    if (VKeyCode == 0xFC  ) KeyCmd = "VK_NONAME";
    if (VKeyCode == 0xFD  ) KeyCmd = "VK_PA1";
    if (VKeyCode == 0xFE  ) KeyCmd = "VK_OEM_CLEAR";
    if (KeyCmd == "") KeyCmd = "Unknown Keyboard Command";
    return KeyCmd;
}

//ParseAcceleratorTable
//Syntax: ParseAccel (ea address_of_resource_to_parse, ea address_of_next_resource)
static ParseAccel(BytePtr, Next_Resource)
{
    auto VKType, VKeyCode, CmdID, VKMod, KeyType, KeyCmd, KeyMod;
    KeyType = " ";
    ExtLinA( BytePtr, 1, "     ----- Start of Accelerator Table -----");
    while ( BytePtr + 7 < Next_Resource)   {
        VKType = Word(BytePtr); //Get Virtual Key Type
        VKeyCode = Word(BytePtr + 2); //Get Virtual Key Code
        CmdID = Word(BytePtr + 4); //Get Command ID for Accelerator
        VKMod = Word(BytePtr + 6); //Get Key Modifier
        MakeQword(BytePtr);
        if ((VKType - 10) > 0) {
            KeyType = KeyType + "ALT ";
            VKType = VKType - 10;
        }
        if ((VKType - 8) > 0){
            KeyType = KeyType + "CTRL ";
            VKType = VKType - 8;
        }
        if ((VKType - 1) > 0){
            KeyType = KeyType + "VIRTKEY ";
            VKType = VKType - 1;
        }
        else KeyType = "ASCII ";
        KeyCmd = IDVKeyCode( VKeyCode );
        KeyMod = IDVKeyCode( VKMod );
        ExtLinB( BytePtr, 0, KeyCmd + " Type: " + KeyType +  " Modifier: " + KeyMod +
                " Command ID: " + ltoa(CmdID,16) );
        BytePtr = BytePtr + 8;
    }
    ExtLinB( BytePtr - 1, 1, "     ------ End of Accelerator Table ------     ");
    ExtLinB( BytePtr - 1, 2, "  ");
}

//ParseImageData
static ParseImageData(BytePtr, ImgType, ImgNum)
{
    auto NumRGBElements, x, ImageSize, ImageEnd;
    ExtLinA(BytePtr, 1, "========== Image Data for " + ImgType + ltoa(ImgNum, 10) + " ==========");
    BytePtr = MakeDword_Cmt(BytePtr, "Size " + ltoa(Dword(BytePtr), 16));
    BytePtr = MakeWord_Cmt(BytePtr, "Width " + ltoa(Word(BytePtr), 10));
    BytePtr = MakeWord_Cmt(BytePtr, "Height " + ltoa(Word(BytePtr), 10));
    BytePtr = MakeWord_Cmt(BytePtr, "Color Planes " + ltoa(Word(BytePtr), 10));
    NumRGBElements = Word(BytePtr);
    BytePtr = MakeWord_Cmt(BytePtr, "Bit Count " + ltoa(Word(BytePtr), 10));
    ImageSize = Dword(BytePtr);
    BytePtr = MakeDWord_Cmt(BytePtr, "Size Of Image " + ltoa(ImageSize, 16));
    ExtLinA(BytePtr, 1, "__________Win32 Color Table__________");
    for(x = 0; x <= NumRGBElements; x = x + 1){
        MakeDWord(BytePtr);
        ExtLinB(BytePtr, 0, ltoa(x, 16) + ": Red " + ltoa(Byte(BytePtr + 2), 10) + " Green " + ltoa(Byte(BytePtr + 1), 10) +
            " Blue " + ltoa(Byte(BytePtr), 10));
        BytePtr = BytePtr + 4;
    }
    ExtLinA(BytePtr, 0, "_____________________________________");
    BytePtr = MakeByte_Cmt(BytePtr, "XOR Mask");
    BytePtr = MakeByte_Cmt(BytePtr, "AND Mask");
    ImageEnd = BytePtr + ImageSize;
    ExtLinB(ImageEnd, 1, "==================================================");
}

//ParseBitmapHeader: Currently for Icons & Cursors
static ParseBitmapHdr(BytePtr, Next_Resource)
{
    auto ImgType, Count, ImageOffset, x;
    BytePtr = MakeWord_Cmt(BytePtr, "Reserved");
    ImgType = IDResource( Word(BytePtr));
    BytePtr = MakeWord_Cmt(BytePtr, "Resource Type " + ImgType);
    Count = Word( BytePtr);
    BytePtr = MakeWord_Cmt(BytePtr, "Number of Resources "+ ltoa(Count, 10));
    //____Each Entry: for 1 to Count
    for( x = 1; x <= Count; x = x + 1){
        ExtLinA(BytePtr, 1, "      ______" + ImgType + " " + ltoa(x, 10) + "______      ");
        BytePtr = MakeByte_Cmt(BytePtr, "Width " + ltoa(Byte(BytePtr), 10));
        BytePtr = MakeByte_Cmt(BytePtr, "Height " + ltoa(Byte(BytePtr), 10));
        BytePtr = MakeByte_Cmt(BytePtr, "Color Count " + ltoa(Byte(BytePtr), 10));
        BytePtr = MakeByte_Cmt(BytePtr, "Reserved");
        if ( ImgType == "Cursor") BytePtr = MakeWord_Cmt(BytePtr, "XHotSpot " + ltoa(Word(BytePtr), 10));
        else BytePtr = MakeWord_Cmt(BytePtr, "Color Panes " + ltoa(Word(BytePtr), 10));
        if ( ImgType == "Cursor") BytePtr = MakeWord_Cmt(BytePtr, "YHotSpot " + ltoa(Word(BytePtr), 10));
        else BytePtr = MakeWord_Cmt(BytePtr, "Bit Count " + ltoa(Word(BytePtr), 10));
        BytePtr = MakeDword_Cmt(BytePtr, "Bytes in Resource: " + ltoa(Dword(BytePtr), 16));
        ImageOffset = MakeDataOffset(BytePtr);
        BytePtr = MakeDword_Cmt(BytePtr, "Image Data Offset: " + atoa(ImageOffset));
        ParseImageData(ImageOffset, ImgType, x);
    }
}

//ParseIcon: Formats Icon Resource
//Syntax: ParseIcon(ea address_of_resource_to_parse, ea address_of_next_resource)
static ParseIcon(BytePtr, Next_Resource)
{
        SetLongPrm(INF_STRTYPE, ASCSTR_UNICODE);
        while ( BytePtr < Next_Resource) {
            if ( Byte(BytePtr) == 0 ) BytePtr = StripZeros(BytePtr);
            MakeStr(BytePtr + 2, BytePtr + (2 * (Byte(BytePtr) + 1)));
            BytePtr = BytePtr + ( 2 * Byte(BytePtr)) + 2;
        }
        //word reserved (0)
        //word idtype (1 = icon)
        //word # of images
        //per icon:
        //byte  width in pixels of the image
        //byte height in pixels of the image
        //byte number of colors in the image (0 if >= 8bpp)
        //byte reserved
        //word wPlanes:Color Panes
        //word Bitcount: Bits per pixel
        //dword # of bytes in this resource  (size)
        //word ID
}

static ParseMenu(BytePtr, Next_Resource)
{
      auto MenuID, enum_id;
      enum_id = AddEnum( (GetEnumQty()+1), "App_Menu", FF_0NUMH );
      SetLongPrm(INF_STRTYPE, ASCSTR_UNICODE);
      MakeDword(BytePtr);
      MakeComm(BytePtr, "MenuHeader");
      BytePtr = BytePtr + 4;
      while ( BytePtr < Next_Resource) {
        MakeWord(BytePtr);
        if (Word(BytePtr) == 0x10 || Dword(BytePtr-4) == 0x100000) {
            MakeComm( BytePtr, "POPUP Menu Item");
            BytePtr = BytePtr + 2;
            MakeStr(BytePtr, -1);
            MakeName(BytePtr, "Popup_Menu_" + Name(BytePtr) );
            ExtLinA(BytePtr, 0, "--------------------------------------------------");
            BytePtr = ItemEnd(BytePtr);
        }
        else if (Word(BytePtr) == 0x00) {
            MakeComm( BytePtr, "SEPARATOR");
            BytePtr = BytePtr + 2;
        }
        else{
            MenuID = Word(BytePtr);
            MakeComm(BytePtr, "Menu Item ID: " + ltoa(MenuID, 10));
            BytePtr = BytePtr + 2;
            MakeStr(BytePtr, -1);
            MakeName(BytePtr, "Menu_Item" + ltoa(MenuID, 10));
            AddConst(enum_id, "Menu_Item_" + Name(BytePtr), MenuID);
            BytePtr = ItemEnd(BytePtr);
        }
      MakeDword(BytePtr);
      MakeComm( BytePtr, "FLAGS");
      BytePtr = BytePtr + 4;
      }
        //while ( BytePtr < Next_Resource) {
           // while ( Byte(BytePtr) == 0x0 && BytePtr < Next_Resource) BytePtr = StripZeros( BytePtr );
           // MakeByte(BytePtr);
           // MenuID = Byte(BytePtr);
           // MakeName(BytePtr,
           // BytePtr = BytePtr + 1;
           // while (Byte(BytePtr) < 0x20 || Byte(BytePtr) > 0x126) {
           //     MakeByte(BytePtr);
           //     BytePtr = BytePtr + 1;
           // }
           // if ( BytePtr + 1 >= Next_Resource) break;
           // ExtLinA(BytePtr, 1, "Menu ID: " + ltoa(MenuID, 10));
           // MakeStr(BytePtr, -1);
           // MenuID = "";
           // BytePtr = ItemEnd(BytePtr);
       // }
}

static ResolveDlgMask(DlgStyle)
{
    auto DialogStyle;
    DialogStyle = " ";
    if ((DlgStyle - 0x80000000) > 0) {
            DialogStyle = DialogStyle + "WS_POPUP ";
            DlgStyle = DlgStyle - 0x80000000;
        }
    if ((DlgStyle - 0x40000000) > 0) {
            DialogStyle = DialogStyle + "WS_CHILD ";
            DlgStyle = DlgStyle - 0x40000000;
        }
    if ((DlgStyle - 0x20000000) > 0) {
            DialogStyle = DialogStyle + "WS_MINIMIZE ";
            DlgStyle = DlgStyle - 0x200000000;
        }
    if ((DlgStyle - 0x10000000) > 0) {
            DialogStyle = DialogStyle + "WS_MAXIMIZE ";
            DlgStyle = DlgStyle - 0x10000000;
        }
    if ((DlgStyle - 0x08000000) > 0) {
            DialogStyle = DialogStyle + "WS_DISABLED ";
            DlgStyle = DlgStyle - 0x08000000;
        }
    if ((DlgStyle - 0x04000000) > 0) {
            DialogStyle = DialogStyle + "WS_CLIPSIBLINGS ";
            DlgStyle = DlgStyle -0x04000000;
        }
    if ((DlgStyle -0x02000000) > 0) {
            DialogStyle = DialogStyle + "WS_CLIPCHILDREN ";
            DlgStyle = DlgStyle - 0x02000000;
        }
    if ((DlgStyle - 0x01000000) > 0) {
            DialogStyle = DialogStyle + "WS_MAXIMIZE ";
            DlgStyle = DlgStyle - 0x01000000;
        }
    if ((DlgStyle - 0x00C00000) > 0) {
            DialogStyle = DialogStyle + "WS_CAPTION ";
            DlgStyle = DlgStyle - 0x00C00000;
        }
    if ((DlgStyle - 0x00800000) > 0) {
            DialogStyle = DialogStyle + "WS_BORDER ";
            DlgStyle = DlgStyle - 0x00800000;
        }
    if ((DlgStyle - 0x00400000) > 0) {
            DialogStyle = DialogStyle + "WS_DLGFRAME  ";
            DlgStyle = DlgStyle - 0x00400000;
        }
    if ((DlgStyle - 0x00200000) > 0) {
            DialogStyle = DialogStyle + "WS_VSCROLL ";
            DlgStyle = DlgStyle - 0x00200000;
        }
    if ((DlgStyle - 0x00100000) > 0) {
            DialogStyle = DialogStyle + "WS_HSCROLL ";
            DlgStyle = DlgStyle - 0x00100000;
        }
    if ((DlgStyle - 0x00080000) > 0) {
            DialogStyle = DialogStyle + "WS_SYSMENU ";
            DlgStyle = DlgStyle - 0x00080000;
        }
    if ((DlgStyle - 0x00040000) > 0) {
            DialogStyle = DialogStyle + "WS_THICKFRAME ";
            DlgStyle = DlgStyle - 0x00040000;
        }
    if ((DlgStyle - 0x00020000) > 0) {
            DialogStyle = DialogStyle + "WS_MINIMIZEBOX  ";
            DlgStyle = DlgStyle - 0x00020000;
        }
    if ((DlgStyle - 0x00010000) > 0) {
            DialogStyle = DialogStyle + "WS_MAXIMIZEBOX ";
            DlgStyle = DlgStyle - 0x00010000;
        }
    if ((DlgStyle - 0x00020000) > 0) {
            DialogStyle = DialogStyle + "WS_GROUP ";
            DlgStyle = DlgStyle - 0x00020000;
        }
    if ((DlgStyle - 0x00010000) > 0) {
            DialogStyle = DialogStyle + "WS_TABSTOP ";
            DlgStyle = DlgStyle - 0x00010000;
        }
    if ((DlgStyle - 0x100) > 0) {
            DialogStyle = DialogStyle + "NOIDLEMESSAGE ";
            DlgStyle = DlgStyle - 0x100;
        }
    if ((DlgStyle - 0x80) > 0) {
            DialogStyle = DialogStyle + "MODALFRAME ";
            DlgStyle = DlgStyle - 0x80;
    }
    if ((DlgStyle - 0x40) > 0) {
            DialogStyle = DialogStyle + "SETFONT ";
            DlgStyle = DlgStyle - 0x40;
        }
    if ((DlgStyle - 0x20) > 0) {
            DialogStyle = DialogStyle + "LOCALEDIT ";
            DlgStyle = DlgStyle - 0x20;
        }
    if ((DlgStyle - 0x02) > 0){
            DialogStyle = DialogStyle + "SYSMODAL ";
            DlgStyle = DlgStyle - 0x02;
        }
    if ((DlgStyle - 0x01) > 0){
            DialogStyle = DialogStyle + "ABSALIGN ";
            DlgStyle = DlgStyle - 0x01;
    }
    else DialogStyle = DialogStyle + "WS_OVERLAPPED ";
    return DialogStyle;
}

static ResolveCtrlMask( CtrlMask )
{
    //if blahblhblah;
 //* Edit Control Styles
//#define ES_LEFT             0x0000L
//#define ES_CENTER           0x0001L
//#define ES_RIGHT            0x0002L
//#define ES_MULTILINE        0x0004L
//#define ES_UPPERCASE        0x0008L
//#define ES_LOWERCASE        0x0010L
//#define ES_PASSWORD         0x0020L
//#define ES_AUTOVSCROLL      0x0040L
//#define ES_AUTOHSCROLL      0x0080L
//#define ES_NOHIDESEL        0x0100L
//#define ES_OEMCONVERT       0x0400L
//#define ES_READONLY         0x0800L
//#define ES_WANTRETURN       0x1000L
//#define ES_NUMBER           0x2000L
 //* Button Control Styles
//#define BS_PUSHBUTTON       0x00000000L
//#define BS_DEFPUSHBUTTON    0x00000001L
//#define BS_CHECKBOX         0x00000002L
//#define BS_AUTOCHECKBOX     0x00000003L
//#define BS_RADIOBUTTON      0x00000004L
//#define BS_3STATE           0x00000005L
//#define BS_AUTO3STATE       0x00000006L
//#define BS_GROUPBOX         0x00000007L
//#define BS_USERBUTTON       0x00000008L
//#define BS_AUTORADIOBUTTON  0x00000009L
//#define BS_OWNERDRAW        0x0000000BL
//#define BS_LEFTTEXT         0x00000020L
//#define BS_TEXT             0x00000000L
//#define BS_ICON             0x00000040L
//#define BS_BITMAP           0x00000080L
//#define BS_LEFT             0x00000100L
//#define BS_RIGHT            0x00000200L
//#define BS_CENTER           0x00000300L
//#define BS_TOP              0x00000400L
//#define BS_BOTTOM           0x00000800L
//#define BS_VCENTER          0x00000C00L
//#define BS_PUSHLIKE         0x00001000L
//#define BS_MULTILINE        0x00002000L
//#define BS_NOTIFY           0x00004000L
//#define BS_FLAT             0x00008000L
//#define BS_RIGHTBUTTON      BS_LEFTTEXT
 //* Static Control Constants
//#define SS_LEFT             0x00000000L
//#define SS_CENTER           0x00000001L
//#define SS_RIGHT            0x00000002L
//#define SS_ICON             0x00000003L
//#define SS_BLACKRECT        0x00000004L
//#define SS_GRAYRECT         0x00000005L
//#define SS_WHITERECT        0x00000006L
//#define SS_BLACKFRAME       0x00000007L
//#define SS_GRAYFRAME        0x00000008L
//#define SS_WHITEFRAME       0x00000009L
//#define SS_USERITEM         0x0000000AL
//#define SS_SIMPLE           0x0000000BL
//#define SS_LEFTNOWORDWRAP   0x0000000CL
//#define SS_BITMAP           0x0000000EL
//#define SS_OWNERDRAW        0x0000000DL
//#define SS_ENHMETAFILE      0x0000000FL
//#define SS_ETCHEDHORZ       0x00000010L
//#define SS_ETCHEDVERT       0x00000011L
//#define SS_ETCHEDFRAME      0x00000012L
//#define SS_TYPEMASK         0x0000001FL
//#define SS_NOPREFIX         0x00000080L /* Don't do "&" character translation */
//#define SS_NOTIFY           0x00000100L
//#define SS_CENTERIMAGE      0x00000200L
//#define SS_RIGHTJUST        0x00000400L
//#define SS_REALSIZEIMAGE    0x00000800L
//#define SS_SUNKEN           0x00001000L
 //* Dialog Styles
//#define DS_ABSALIGN         0x01L
//#define DS_SYSMODAL         0x02L
//#define DS_LOCALEDIT        0x20L   /* Edit items get Local storage. */
//#define DS_SETFONT          0x40L   /* User specified font for Dlg controls */
//#define DS_MODALFRAME       0x80L   /* Can be combined with WS_CAPTION  */
//#define DS_NOIDLEMSG        0x100L  /* WM_ENTERIDLE message will not be sent */
//#define DS_SETFOREGROUND    0x200L  /* not in win3.1 */
//#define DS_3DLOOK           0x0004L
//#define DS_FIXEDSYS         0x0008L
//#define DS_NOFAILCREATE     0x0010L
//#define DS_CONTROL          0x0400L
//#define DS_CENTER           0x0800L
//#define DS_CENTERMOUSE      0x1000L
//#define DS_CONTEXTHELP      0x2000L
 //* Listbox Styles
//#define LBS_NOTIFY            0x0001L
//#define LBS_SORT              0x0002L
//#define LBS_NOREDRAW          0x0004L
//#define LBS_MULTIPLESEL       0x0008L
//#define LBS_OWNERDRAWFIXED    0x0010L
//#define LBS_OWNERDRAWVARIABLE 0x0020L
//#define LBS_HASSTRINGS        0x0040L
//#define LBS_USETABSTOPS       0x0080L
//#define LBS_NOINTEGRALHEIGHT  0x0100L
//#define LBS_MULTICOLUMN       0x0200L
//#define LBS_WANTKEYBOARDINPUT 0x0400L
//#define LBS_EXTENDEDSEL       0x0800L
//#define LBS_DISABLENOSCROLL   0x1000L
//#define LBS_NODATA            0x2000L
//#define LBS_NOSEL             0x4000L
//#define LBS_STANDARD          (LBS_NOTIFY | LBS_SORT | WS_VSCROLL | WS_BORDER)
// * Combo Box styles
//#define CBS_DROPDOWN          0x0002L
//#define CBS_DROPDOWNLIST      0x0003L
//#define CBS_OWNERDRAWFIXED    0x0010L
//#define CBS_OWNERDRAWVARIABLE 0x0020L
//#define CBS_AUTOHSCROLL       0x0040L
//#define CBS_OEMCONVERT        0x0080L
//#define CBS_SORT              0x0100L
//#define CBS_HASSTRINGS        0x0200L
//#define CBS_NOINTEGRALHEIGHT  0x0400L
//#define CBS_DISABLENOSCROLL   0x0800L
//#define CBS_UPPERCASE           0x2000L
//#define CBS_LOWERCASE           0x4000L


}

static ParseDlgItem( BytePtr, Next_Resource )
{
    auto Mask;
    SetLongPrm(INF_STRTYPE, ASCSTR_UNICODE);
     while ( Byte(BytePtr) != 0x50 && BytePtr < Next_Resource ) BytePtr = BytePtr + 1;
     if ( BytePtr + 22 >= Next_Resource ) return;
    Mask = Dword(BytePtr-3);
    // ResolveCtrlMask( Mask );
    BytePtr = MakeDword_Cmt( BytePtr - 3, "Control Style: " + ltoa(Mask, 16));
    BytePtr = MakeDword_Cmt( BytePtr, "Reserved");
    BytePtr = MakeWord_Cmt( BytePtr, "X Location: " + ltoa(Word(BytePtr), 10));
    BytePtr = MakeWord_Cmt( BytePtr, "Y Location: " + ltoa(Word(BytePtr), 10));
    BytePtr = MakeWord_Cmt( BytePtr, "X Size: " + ltoa(Word(BytePtr), 10));
    BytePtr = MakeWord_Cmt( BytePtr, "Y Size: " + ltoa(Word(BytePtr), 10));
    BytePtr = MakeWord_Cmt( BytePtr, "Control Number: " + ltoa(Word(BytePtr), 10));
    BytePtr = MakeWord_Cmt( BytePtr, "Reserved");
    BytePtr = MakeWord_Cmt( BytePtr, "Control Type: " + ltoa(Word(BytePtr), 10));
    if ( Word(BytePtr) != 0x0000) MakeStr( BytePtr, -1); // Unicode ControlText
    MakeComm(BytePtr, "Control Text");
    BytePtr = ItemEnd(BytePtr);
    return BytePtr;
}

static ParseDialog(BytePtr, Next_Resource)
{
    auto DlgStyle, x, Num_Controls;
    SetLongPrm(INF_STRTYPE, ASCSTR_UNICODE);
    BytePtr = MakeWord_Cmt(BytePtr, "Dialog #");
    DlgStyle = ResolveDlgMask(Dword(BytePtr));
    BytePtr = MakeDword_Cmt(BytePtr, "Dialog Style: " + DlgStyle);
    BytePtr = MakeWord_Cmt(BytePtr, "Reserved");
    Num_Controls = Word(BytePtr);
    BytePtr = MakeWord_Cmt(BytePtr, "Number of Controls: " + ltoa(Num_Controls, 10));
    BytePtr = MakeWord_Cmt(BytePtr, "X Location");
    BytePtr = MakeWord_Cmt(BytePtr, "Y Location");
    BytePtr = MakeWord_Cmt(BytePtr, "X Size");
    BytePtr = MakeWord_Cmt(BytePtr, "Y Size");
    BytePtr = MakeDword_Cmt(BytePtr, "Reserved");
    MakeComm(BytePtr, "Caption");
    MakeStr(BytePtr, -1);
    BytePtr = ItemEnd(BytePtr);
    BytePtr = MakeWord_Cmt(BytePtr, "Reserved");
    BytePtr = MakeWord_Cmt(BytePtr, "Font Number ");
    MakeStr(BytePtr, -1);
    BytePtr = ItemEnd(BytePtr);
    for (x = 1; x <= Num_Controls; x = x + 1) {
        ExtLinA(BytePtr, 0, "__________Control__" + ltoa(x, 10) + "__________");
        BytePtr = ParseDlgItem( BytePtr, Next_Resource );
        while ( Byte(BytePtr) == 0 && BytePtr < Next_Resource) BytePtr = StripZeros( BytePtr );
    }
}

static ParseVerInfo(BytePtr, Next_Resource)
{
    auto EndOfString;
    SetLongPrm(INF_STRTYPE, ASCSTR_UNICODE);
    // 3x DW, Unicode
    //for ( x = 1; x <= 31; x = x + 1){ MakeWord(BytePtr); BytePtr = BytePtr + 2;}
    while ( BytePtr < Next_Resource) {
        while ( Byte(BytePtr) == 0 && BytePtr < Next_Resource) BytePtr = StripZeros( BytePtr );
        while (Byte(BytePtr) < 0x20 || Byte(BytePtr) > 0x126) {
                if ( BytePtr + 1 >= Next_Resource) break;
                MakeByte(BytePtr);
                BytePtr = BytePtr + 1;
        }
        if ( BytePtr + 1 >= Next_Resource) break;
        MakeStr(BytePtr, -1);
        BytePtr = ItemEnd( BytePtr );
    }
}

//ParseUnknownResource: Formats Resource Data into an 8-byte-per-line (QWord) output
//Syntax: ParseUnkRes(ea address_of_resource_to_parse, ea address_of_next_resource)
static ParseUnkRes(BytePtr, Next_Resource)
{
    for ( BytePtr; BytePtr < Next_Resource; BytePtr = BytePtr + 1) {
        if (BytePtr + 7 < Next_Resource){
            MakeQword(BytePtr);
            BytePtr = BytePtr + 7;
        }
                else MakeByte(BytePtr);
     }
}

//ParseLeafNode adds an anterior comment, calls a parsing routine dependent on resource type if the
//display resource flag (_DISPLAY_RESOURCES_) is set.
//Syntax: ParseLeafNode( ea add_of_resource, char resource_type, long resource_ID_number)
static ParseLeafNode(BytePtr, Resource_Type, Resource_Num)
{
    auto Next_Resource, Data_Offset;
    Data_Offset = MakeDataOffset(BytePtr);
    ExtLinA(BytePtr, 0, "------------------------------------------------------------------" );
    ExtLinA(BytePtr, 1, "-- Pointer to " + Resource_Type + " at address " + ltoa(Data_Offset, 16) +
            " of size " + ltoa(Dword(BytePtr + 4), 16) + " --");
    Next_Resource = MakeDataOffset(BytePtr + 16);
    MakeStruct(BytePtr, "DataEntry");
    BytePtr = Data_Offset;
    ExtLinA(BytePtr, 0, Resource_Type + " " + ltoa(Resource_Num, 10));
    if ( _DISPLAY_RESOURCES_ ==  1) {
        if ( Resource_Type == "String_Table" ) ParseStrTable( BytePtr, Next_Resource, Resource_Num );
        else if ( Resource_Type == "Menu" ) ParseMenu( BytePtr, Next_Resource );
        else if ( Resource_Type == "Accelerator_Table") ParseAccel( BytePtr, Next_Resource );
        else if ( Resource_Type == "Version" ) ParseVerInfo( BytePtr, Next_Resource );
        else if ( Resource_Type == "Dialog" ) ParseDialog( BytePtr, Next_Resource );
        //else if ( Resource_Type == "Cursor" || Resource_Type == "Icon") ParseBitmapHdr( BytePtr, Next_Resource);
        else ParseUnkRes( BytePtr, Next_Resource );
    }
}

//ParseSubdirectoryData parses resource subdirectory entries, then either continues parsing the
//resource directory tree (if the recursion flag _RECURSE_ is set), or calls ParseLeafNode to
//format and display the resource (if the entry is a pointer to resource data)
//Syntax: ParseSubDirData( ea addess_of_resource_subdir_entry, char resource_type, , long num, long nest )
static ParseSubDirData(BytePtr, Resource_Type, Resource_Num, nest)
{
    auto Next_Resource;
    if (nest == 1) Resource_Num = Dword(BytePtr);
    MakeStruct(BytePtr, "DirEntry");
    Next_Resource = ResolveOffset(BytePtr + 8);
    if( Next_Resource >= SegEnd(BytePtr)) Next_Resource = SegEnd(BytePtr);
    BytePtr = BytePtr + 4;
    if ( Byte(BytePtr + 3) == 0x80) {
        ExtLinB(BytePtr, 0, "  --- Offset to Subdirectory at " + ltoa(ResolveOffset(BytePtr),16) + " ---");
        if ( _RECURSE_ == 1) ParseSubDirHdr( ResolveOffset(BytePtr), Resource_Type, Resource_Num, nest );
    }
    else ParseLeafNode(ResolveOffset(BytePtr), Resource_Type, Resource_Num);
}

//ParseDirectoryHeader parses & comments a directory header and returns the total # of dir data entries
//Syntax: ParseDirHdr( ea starting_address_of_dir_header )
//Returns: ea total_number_of_data_entries
static ParseDirHdr(BytePtr)
{
    auto Num_Named, Num_IDs, Total_Res_Entries;
    MakeStruct(BytePtr, "ResDirHeader");
    Num_Named = Word(BytePtr + 12);
    Num_IDs = Word(BytePtr + 14 );
    Total_Res_Entries = Num_Named + Num_IDs;
    return Total_Res_Entries;
}

//ParseSubdirectoryHeader calls ParseDirHdr to parse the header for a subdirectory, then calls
//ParseSubDirData for each resource subdirectory entry
//Syntax: ParseSubDirHdr( ea address_of_subdirectory_header, char resource_type, long num, long nest )
static ParseSubDirHdr(BytePtr, Resource_Type, Resource_Num, nest)
{
    auto Total_Res_Entries, x;
    nest = nest + 1;
    MakeName( BytePtr, "rsrc" + Resource_Type + "Dir" + atoa(BytePtr));
    ExtLinA( BytePtr, 0, "-------------------- Start " + Resource_Type + " Header --------------------");
    Total_Res_Entries = ParseDirHdr(BytePtr);
    BytePtr = BytePtr + 14;
    ExtLinB(BytePtr, 0, "Total Number of " + Resource_Type + " Entries: " + ltoa(Total_Res_Entries,10));
    ExtLinB( BytePtr, 1, "--------------------- End " + Resource_Type + " Header ---------------------");
    BytePtr = BytePtr + 2;
    for ( x = 1; x <= Total_Res_Entries; x = x + 1) {
        ParseSubDirData(BytePtr, Resource_Type, Resource_Num, nest);
        BytePtr = BytePtr + 8;
    }
    nest = nest - 1;
}

//ParseRootDirData parses the "resource directory entries" for the root .rsrc directory, then
//calls ParseSubDirHdr to handle subsequent subdirectories if the recursion flag _RECURSE_ is set
//Syntax: ParseRootData( ea address_of_resource_directory_entry)
static ParseRootData(BytePtr)
{
    auto Resource_Type, Resource_Num, nest;
    nest = 0;
    Resource_Num = 0;
    Resource_Type = ID_Resource(Dword(BytePtr));
    MakeStruct(BytePtr, "DirEntry");
    ExtLinA(BytePtr, 0, "        Resource Type: " + Resource_Type);
    BytePtr = BytePtr + 4;
    if ( Byte(BytePtr + 3) == 0x80) {
        ExtLinB(BytePtr, 0, "  --- Offset to " + Resource_Type + " Subdirectory at " + ltoa(ResolveOffset(BytePtr),16) + " ---");
        if ( _RECURSE_ == 1) ParseSubDirHdr(ResolveOffset(BytePtr), Resource_Type, Resource_Num, nest);
    }
}

//ParseRootDirectoryHeader calls ParseDirHdr to parse & comments the root directory of the .rsrc segment,
//then calls ParseRootData for each entry in its listing
//Syntax: ParseRootHdr( ea starting_address_of_rsrc_segment)
static ParseRootHdr(BytePtr)
{
    auto Total_Res_Entries, x, DirHeaderID, DirEntryID, DataEntryID;
    DirHeaderID = AddStruc( -1, "ResDirHeader" );
    AddStrucMember( DirHeaderID, "Characteristics", 0x0000, FF_DWRD + FF_DATA, -1, 1);
    AddStrucMember( DirHeaderID, "TimeDate", 0x0004, FF_DWRD + FF_DATA, -1, 1);
    AddStrucMember( DirHeaderID, "Version", 0x0008, FF_DWRD + FF_DATA, -1, 1);
    AddStrucMember( DirHeaderID, "NamedEntries", 0x000C, FF_WORD + FF_DATA, -1, 1);
    AddStrucMember( DirHeaderID, "IDEntries", 0x000E, FF_WORD + FF_DATA, -1, 1);
    DirEntryID = AddStruc( -1, "DirEntry");
    AddStrucMember( DirEntryID, "Name", 0x0000, FF_DWRD + FF_DATA, -1, 1);
    AddStrucMember( DirEntryID, "OffsetToData", 0x0004, FF_DWRD + FF_DATA, -1, 1);
    DataEntryID = AddStruc( -1, "DataEntry" );
    AddStrucMember( DataEntryID, "OffsetToData", 0x0000, FF_DWRD + FF_DATA, -1, 1);
    AddStrucMember( DataEntryID, "Size", 0x0004, FF_DWRD + FF_DATA, -1, 1);
    AddStrucMember( DataEntryID, "CodePage", 0x0008, FF_DWRD + FF_DATA, -1, 1);
    AddStrucMember( DataEntryID, "Reserved", 0x000C, FF_DWRD + FF_DATA, -1, 1);
    MakeName( BytePtr, "rsrcRootDirHeader");
    Total_Res_Entries = ParseDirHdr(BytePtr);
    BytePtr = BytePtr + 14;
    ExtLinB(BytePtr, 0, "Total Number of Entries: " + ltoa(Total_Res_Entries,10));
    ExtLinB(BytePtr, 1, "---------------------- End Of Resource Root Directory -------------------");
    ExtLinB(BytePtr, 2, "=========================================================================");
    BytePtr = BytePtr + 2;
    for ( x = 1; x <= Total_Res_Entries; x = x + 1) {
        ParseRootData(BytePtr);
        BytePtr = BytePtr + 8;
    }
}



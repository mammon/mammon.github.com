
//------------------------------------------------------------------------------------------------------
//Strings.idc (rev 2.27): Imports #defines from a .h file, adds as enums
//Note:  This script prompts the user for a header file (*.h), then parses the
//       file looking for #define statements: these are then converted to members
//       of enum "Defines".
//Bugs: Only the first instance of any value will be preserved; all others will be
//      discarded with an error as you can have only one instance of any value (or
//      any name) in a single enumeration. A prompt has been added for the user to
//      name the enumerations for the header file, so that any duplicate enum values
//      can be added to a different file and enumerated under a different "enum name."
//
// code by mammon_ All rights reversed, use as you see fit.....
//------------------------------------------------------------------------------------------------------

#include <idc.idc>

static strip_spaces( BytePtr, hHeaderFile){
       auto tempc;
       fseek( hHeaderFile, BytePtr, 0);
       tempc = fgetc(hHeaderFile);
       while ( tempc == 0x20) {
        BytePtr = BytePtr + 1;
        fseek( hHeaderFile, BytePtr, 0);
        tempc = fgetc(hHeaderFile);
       }
       return BytePtr;
}

static FindStringEnd( StrName ){
    auto x, tempc;
    for ( x = 1; x < strlen(StrName); x = x + 1) {
        tempc = substr( StrName, x-1, x);
        if ( tempc == " ") {
            return substr( StrName, 0, x);
        }
    }
    return substr( StrName, 0, strlen(StrName));
}

static FixString( StrName ){
    auto x, tempc, newname;
    newname="def";    //set newname to type character
    for ( x = 1; x < strlen(StrName); x = x + 1) {
        tempc = substr( StrName, x-1, x);
        if ( tempc != "_") {
            newname = newname + tempc;
        }
    }
    return newname;
}

static main(){
    auto HeaderFile, hHeaderFile, fLength, BytePtr, first_str, second_str, third_str, define_val;
    auto enum_id, tempc1, x, y, errcode, define_name, FilePtr, define_str, enum_name;
    FilePtr = 0;
    Message("\nStart Conversion\n");
    HeaderFile = AskFile( "*.h", "Choose a header file to parse:");
    enum_name = AskStr("Defines", "Enter a name for the enumerations (alpha only, eg 'VMMDefines'):");
    hHeaderFile = fopen( HeaderFile, "r");
    fLength = filelength(hHeaderFile);
    if( fLength == -1) Message( "Bad File Length!\n");
    enum_id = AddEnum( GetEnumQty() + 1, enum_name, FF_0NUMH);
    if ( enum_id == -1) {
        enum_id = GetEnum( enum_name );
        if(enum_id == -1) Message("Enum #Defines not created/not found\n");
    }
    SetEnumCmt( enum_id, "#define from " + HeaderFile, 1);
    while(FilePtr < fLength ){
        FilePtr = strip_spaces( FilePtr, hHeaderFile );
        BytePtr = FilePtr;
        errcode = fseek( hHeaderFile, BytePtr, 0 );
        if ( errcode != 0) break;
        first_str = readstr( hHeaderFile );
        if ( first_str == -1 ) {
            Message( "End of file! \n" );
            break;
        }
        else if ( substr(first_str, 0, 7) == "#define" || substr( first_str, 0, 7) == "#DEFINE" ) {
            FilePtr = FilePtr + strlen( first_str );
            BytePtr = BytePtr + 7;
            BytePtr = strip_spaces( BytePtr, hHeaderFile );
            errcode = fseek( hHeaderFile, BytePtr, 0 );
            if ( errcode != 0 ) break;
            second_str = readstr( hHeaderFile );
            if ( second_str == -1 ) {
                Message( "End of file after #define!\n" );
                break;
            }
            else {
                define_name = FindStringEnd( second_str );
                define_name = FixString( define_name );
                BytePtr = strip_spaces( BytePtr + strstr( second_str, " " ), hHeaderFile );
                errcode = fseek( hHeaderFile, BytePtr, 0);
                if ( errcode != 0 ) break;
                third_str = readstr( hHeaderFile);
                tempc1 = substr(third_str, 0, 2);
                if ( third_str == -1) {
                    Message( "End of file before value!\n");
                    break;
                }
                else if ( tempc1 == "0x" || tempc1 == "0X") {
                  define_str = FindStringEnd( third_str );
                    define_val = xtol( define_str );
                    errcode = AddConst( enum_id, define_name, define_val);
                    if ( errcode == 1 ) Message( "Name " + define_name + " bad or already used in program!\n");
                    if ( errcode == 2 ) Message( "Value " + define_str + " already used in program!\n");
                    if ( errcode == 3 ) Message( "Bad enumID!\n");
                }
            }
        }
        else FilePtr = FilePtr + strlen( first_str);
    }
    Message("\nConversion finished!\n");
}




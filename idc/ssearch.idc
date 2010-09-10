
//ss.idc
//
//code by mammon_ rev 4.02

#include <idc.idc>

static ascii2hex( input_char ){
    auto hex_val;
    if ( input_char == " ") hex_val=0x20;
    if ( input_char == "!") hex_val=0x21;
    if ( input_char == "\"") hex_val=0x22;
    if ( input_char == "#") hex_val=0x23;
    if ( input_char == "$") hex_val=0x24;
    if ( input_char == "%") hex_val=0x25;
    if ( input_char == "&") hex_val=0x26;
    if ( input_char == "'") hex_val=0x27;
    if ( input_char == "(") hex_val=0x28;
    if ( input_char == ")") hex_val=0x29;
    if ( input_char == "*") hex_val=0x2A;
    if ( input_char == "+") hex_val=0x2B;
    if ( input_char == ",") hex_val=0x2C;
    if ( input_char == "-") hex_val=0x2D;
    if ( input_char == ".") hex_val=0x2E;
    if ( input_char == "/") hex_val=0x2F;
    if ( input_char == "0") hex_val=0x30;
    if ( input_char == "1") hex_val=0x31;
    if ( input_char == "2") hex_val=0x32;
    if ( input_char == "3") hex_val=0x33;
    if ( input_char == "4") hex_val=0x34;
    if ( input_char == "5") hex_val=0x35;
    if ( input_char == "6") hex_val=0x36;
    if ( input_char == "7") hex_val=0x37;
    if ( input_char == "8") hex_val=0x38;
    if ( input_char == "9") hex_val=0x39;
    if ( input_char == ":") hex_val=0x3A;
    if ( input_char == ";") hex_val=0x3B;
    if ( input_char == "<") hex_val=0x3C;
    if ( input_char == "=") hex_val=0x3D;
    if ( input_char == ">") hex_val=0x3E;
    if ( input_char == "?") hex_val=0x3F;
    if ( input_char == "@") hex_val=0x40;
    if ( input_char == "A") hex_val=0x41;
    if ( input_char == "B") hex_val=0x42;
    if ( input_char == "C") hex_val=0x43;
    if ( input_char == "D") hex_val=0x44;
    if ( input_char == "E") hex_val=0x45;
    if ( input_char == "F") hex_val=0x46;
    if ( input_char == "G") hex_val=0x47;
    if ( input_char == "H") hex_val=0x48;
    if ( input_char == "I") hex_val=0x49;
    if ( input_char == "J") hex_val=0x4A;
    if ( input_char == "K") hex_val=0x4B;
    if ( input_char == "L") hex_val=0x4C;
    if ( input_char == "M") hex_val=0x4D;
    if ( input_char == "N") hex_val=0x4E;
    if ( input_char == "O") hex_val=0x4F;
    if ( input_char == "P") hex_val=0x50;
    if ( input_char == "Q") hex_val=0x51;
    if ( input_char == "R") hex_val=0x52;
    if ( input_char == "S") hex_val=0x53;
    if ( input_char == "T") hex_val=0x54;
    if ( input_char == "U") hex_val=0x55;
    if ( input_char == "V") hex_val=0x56;
    if ( input_char == "W") hex_val=0x57;
    if ( input_char == "X") hex_val=0x58;
    if ( input_char == "Y") hex_val=0x59;
    if ( input_char == "Z") hex_val=0x5A;
    if ( input_char == "[") hex_val=0x5B;
    if ( input_char == "\\") hex_val=0x5C;
    if ( input_char == "]") hex_val=0x5D;
    if ( input_char == "^") hex_val=0x5E;
    if ( input_char == "_") hex_val=0x5F;
    if ( input_char == "`") hex_val=0x60;
    if ( input_char == "a") hex_val=0x61;
    if ( input_char == "b") hex_val=0x62;
    if ( input_char == "c") hex_val=0x63;
    if ( input_char == "d") hex_val=0x64;
    if ( input_char == "e") hex_val=0x65;
    if ( input_char == "f") hex_val=0x66;
    if ( input_char == "g") hex_val=0x67;
    if ( input_char == "h") hex_val=0x68;
    if ( input_char == "i") hex_val=0x69;
    if ( input_char == "j") hex_val=0x6A;
    if ( input_char == "k") hex_val=0x6B;
    if ( input_char == "l") hex_val=0x6C;
    if ( input_char == "m") hex_val=0x6D;
    if ( input_char == "n") hex_val=0x6E;
    if ( input_char == "o") hex_val=0x6F;
    if ( input_char == "p") hex_val=0x70;
    if ( input_char == "q") hex_val=0x71;
    if ( input_char == "r") hex_val=0x72;
    if ( input_char == "s") hex_val=0x73;
    if ( input_char == "t") hex_val=0x74;
    if ( input_char == "u") hex_val=0x75;
    if ( input_char == "v") hex_val=0x76;
    if ( input_char == "w") hex_val=0x77;
    if ( input_char == "x") hex_val=0x78;
    if ( input_char == "y") hex_val=0x79;
    if ( input_char == "z") hex_val=0x7A;
    if ( input_char == "{") hex_val=0x7B;
    if ( input_char == "|") hex_val=0x7C;
    if ( input_char == "}") hex_val=0x7D;
    if ( input_char == "~") hex_val=0x7E;
    if ( !(hex_val)) hex_val=0x20;
    return hex_val;
}

static String2Hex( input_string ){
    auto x, input_char, hex_str;
    for(x =1; x<= strlen( input_string ); x = x + 1){
        input_char = substr( input_string, x, x );
        hex_str = hex_str + ltoa(ascii2hex( input_char), 16) ;
    }
    return hex_str;
}

static main(){
    auto x, ea, target_str, target_bytes, target, cur_string, nomatch, is_found, end_seg;
    ea = FirstSeg();
    end_seg = SegEnd(ea);
    target_str = AskStr( "egister", "Enter target string:");
    target_bytes = strlen( target_str );
    target = String2Hex( target_str );
    while ( ea != BADADDR) {
        nomatch = 0;
        //if ( isData(ea)){
                //cur_string = GetOperandValue(ea,1);
                for( x = 1; x <= target_bytes * 2; x = x + 2){
                    if ( Byte(ea) != (xtol( substr( target , x, x + 1) ))  )nomatch = 1;
                    if (nomatch == 1) break;
                }
                if ( nomatch == 0 )  Message( "String found at " + atoa( ea ) + " : " + GetTrueName(ea) + "\n");
       // }
        if (ea == end_seg){
                ea = NextSeg(ea);
                if (ea!=BADADDR) end_seg = SegEnd(ea);
        }
        else ea = NextHead(ea);
    }
    Message( "String Search complete!\n\n" );
}



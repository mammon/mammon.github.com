//-------------------------------------------------------------------------------------------------------
//textsearch.idc: Front-end to "search for text in core" enabling you to enter strings
//
// code by mammon_ All rights reversed
//-------------------------------------------------------------------------------------------------------

#include <idc.idc>

static main()
{
   auto ea, x, y, searchstr, temp_c, binstr, array_id, alphabet, bin_c, cont;
   ea = FirstSeg();
	// ---- Create Array Of ASCII Characters ------------------------
	// ---- Note that the index of each char = its decimal value ----
	array_id = CreateArray("AsciiTable");
	alphabet = "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz";
	y = 48;
	for (x = 0; x < strlen(alphabet); x = x + 1 ) {
	 	SetArrayString( array_id, y, substr(alphabet, x, x+1));
		y = y +1;
	}
	// ---- Prompt User For Search String ----------------------------
    searchstr = AskStr("", "Enter a search string:\n");
	// ---- Cycle through array looking for match --------------------
    for (x = 0; x < strlen(searchstr); x = x + 1 ) {
		temp_c = substr(searchstr, x, x + 1 );
		for( y = GetFirstIndex(AR_STR, array_id); y <= GetLastIndex(AR_STR, array_id); y =  GetNextIndex(AR_STR, array_id, y) ) {
			if (temp_c == GetArrayElement(AR_STR, array_id, y)) {
				bin_c = y;
				break;
			}														//End "If Match"
		} 	  														//End Array Loop
		binstr = form("%s %X", binstr, bin_c);
    }																//End Search String Loop
	Message("Search string is " + binstr + "\n");	//Debug Control
	// -------- "Search" and "Search Again" Loop... --------------------
	cont = 1;
	while (cont==1) {
		ea = FindBinary(ea, 1, binstr);					//Search From ea
		if( ea == -1) {										//If No Hits
			Warning("No more occurrences");				//MessageBox
			cont = 0;
			break;												//Leave
		}
		Jump(ea);                							//Position Cursor At Hit
		cont = AskYN( 1, "Find next occurence?" );	//Search Again?
	}
	// --------- Cleanup and Exit
   Message("\n" + "Search Complete\n");
	DeleteArray(array_id);
}
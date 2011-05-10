//gcc -o hexedit hexedit.c -lncurses
//Linux Hex Editor
//  Basically an extension of the hexdump.c ncurses experiments.
//  Currently has the following problems:
//    * Changes are made to the target file whether you save or not. Must incorporate a tmp file to counteract mmap.
//    * No real effort made at error checking, code commenting, or documentation. Be warned.
//    * Some useful features are not yet written -- Search, Goto, and Elf Header. All in good time.
//    * source is formatted for reading in VI
//Regarding ncurses, if you are not seeing colors check $TERM env variable -- set it from xterm to rxvt
//and this should show colors. If this works you may want to overwrite the xterm entry in termcap with the rxvt one
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <ncurses/curses.h>

#define SIZEofPAGE (16 * (LINES - 4))

//_______________________________GLOBALS
off_t DisplayStartAddr, FileSize, LastPage;
int fd, pos;
char *fname, *MappedFile;
bool InEdit, NotSaved;

//-------------------------utility functions
void BlankLine(int y){
	int i;
	for (i=0; i<COLS; i++){
		move( y,i );
		printw(" ");
	}
	return;
}

void SetupColors() {
	if ( has_colors() ) {
		start_color();
		init_pair( 1, COLOR_WHITE, COLOR_BLUE);
		init_pair( 2, COLOR_CYAN, COLOR_BLUE);
		init_pair( 3, COLOR_GREEN, COLOR_BLUE);
		init_pair( 4, COLOR_YELLOW, COLOR_BLUE);
		init_pair( 5, COLOR_RED, COLOR_CYAN);
		init_pair( 6, COLOR_BLUE, COLOR_CYAN);
		init_pair( 7, COLOR_RED, COLOR_BLUE);
		init_pair( 8, COLOR_BLUE, COLOR_BLUE);
	}
}

int xatoi( char a) {
	int i;
	//assume a is between 0 and F
	if ( a >= '0' && a <= '9') i = a - 48;      //0x30
	else if ( a >= 'A' && a <= 'F') i = a - 55; //0x37
	else i = -1;

	return i;
}

int NextX( int x, bool FORWARD ) {
	int j = x - 12;
	if (FORWARD) {
	  if ( x >= 13 && x <=59) {
		if ( j % 3 == 0) return x + 1;
		else return x;
	  }
	  else if ( x > COLS - 1) return COLS - 1;
	  else if ( x >= COLS - 16) return x;
 	  else return COLS - 16;
	}
	else {
	  if (x < 13) return 13;
	  if ( x >= 13 && x <= 59) {
		  if (j % 3 == 0) return x - 1;
		  else return x;
	  }
	  else if (x >= COLS - 16) return x;
	  else return 59;
	}
}

int LoadFile() {
	if ((fd = open(fname, O_RDWR, NULL)) < 0) {
		perror("open");
		exit(1);
	}
	FileSize = lseek(fd, 0, SEEK_END);
	LastPage = (FileSize - (FileSize % 16)) - ((LINES -5) *16);
	if ((MappedFile = (char *)mmap( NULL, FileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == (caddr_t) -1) {
		perror("mmap");
		exit(1);
	}
	return 0;
}

//----------------------------Menu Functions
int SaveFile(){
	WINDOW *SaveWin;

    msync(MappedFile, FileSize, MS_ASYNC);;
	SaveWin = newwin(3, 20, (LINES / 2) - 2, (COLS / 2) - 10);
	wattron(SaveWin, COLOR_PAIR(2));
	werase(SaveWin);
	mvwprintw(SaveWin, 1, 1, "    File Saved    ");
	wattron(SaveWin, COLOR_PAIR(1));
	box(SaveWin, ACS_VLINE, ACS_HLINE);
	wrefresh(SaveWin);

    getch();
	delwin(SaveWin);
	move(LINES,COLS);
	touchwin(stdscr);
	wrefresh(stdscr);
	return 0;
}

void Goto() {
	WINDOW *GotoWin;

	GotoWin = newwin( 5, 20, (LINES/2) - 2, (COLS/2)-10);
	wattron(GotoWin, COLOR_PAIR(2));
	werase(GotoWin);
	mvwprintw(GotoWin, 2, 2, "Not Implemented");
	wattron(GotoWin, COLOR_PAIR(1));
	box(GotoWin, ACS_VLINE, ACS_HLINE);
	wrefresh(GotoWin);

	getch();
	delwin(GotoWin);
	move(LINES, COLS);
	touchwin(stdscr);
	wrefresh(stdscr);
	return;
}

void Search() {
	WINDOW *SearchWin;

	SearchWin = newwin( 10, 40, (LINES/2) - 5, (COLS/2)-20);
	wattron(SearchWin, COLOR_PAIR(2));
	werase(SearchWin);
	mvwprintw(SearchWin, 4, 2, "Not Implemented");
	wattron(SearchWin, COLOR_PAIR(1));
	box(SearchWin, ACS_VLINE, ACS_HLINE);
	wrefresh(SearchWin);

	getch();
	delwin(SearchWin);
	move(LINES, COLS);
	touchwin(stdscr);
	wrefresh(stdscr);
	return;
}

void ParseELFHeader() {
	WINDOW *ElfWin;

   ElfWin = newwin( 20, 30, (LINES/2) - 10, (COLS/2)-15);
	wattron(ElfWin, COLOR_PAIR(2));
	werase(ElfWin);
	mvwprintw(ElfWin, 10, 3, "Not Implemented");
	wattron(ElfWin, COLOR_PAIR(1));
	box(ElfWin, ACS_VLINE, ACS_HLINE);
	wrefresh(ElfWin);

	getch();
	delwin(ElfWin);
	move(LINES, COLS);
	touchwin(stdscr);
	wrefresh(stdscr);
	return;
}


void HelpWindow() {
	WINDOW *HelpWin;

	int x, y, height = 12, width = 28;
	int StartY= (LINES / 2) - (height / 2);
	int StartX = (COLS / 2) - (width / 2);
	if ((HelpWin = newwin( height, width, StartY, StartX)) ) {
		wattron(HelpWin, COLOR_PAIR(1));
		for (y = 0; y < height; y++){
			move(StartY + y, StartX );
			for (x = 0; x < width ; x++) wprintw(HelpWin, " ");
		}
		wrefresh(HelpWin);
		mvwprintw(HelpWin, 1, 6, "Linux Hex Edit");
		wattron(HelpWin, COLOR_PAIR(3));
		mvwprintw(HelpWin, 2, 4, "Programmer's Editor");
		wattron(HelpWin, COLOR_PAIR(2));
		mvwprintw(HelpWin, 4, 4, "F2: Save File");
		mvwprintw(HelpWin, 5, 4, "F3: Patch File");
		mvwprintw(HelpWin, 6, 4, "F7: Search File");
		mvwprintw(HelpWin, 7, 4, "F8: View ELF Header");
		mvwprintw(HelpWin, 8, 4, "F10: Exit");
		wattron(HelpWin, COLOR_PAIR(4));
		mvwprintw(HelpWin, 10, 6, "Press Any Key...");
		wrefresh(HelpWin);
		wattron(HelpWin, COLOR_PAIR(1));
		box(HelpWin, ACS_VLINE, ACS_HLINE);
		wrefresh(HelpWin);
		getch();
		delwin(HelpWin);
	        move(LINES,COLS);
		touchwin(stdscr);
		wrefresh(stdscr);
	}
	return;
}

void EditWindow() {
	int KeyPress, j, currX = 13, currY = 2;
	char InChar, NewByte, Buf[2];
	off_t ByteAddr;
	InEdit = TRUE;
	StatusBar();
	wrefresh(stdscr);
	leaveok(stdscr, FALSE);
	move(currY, currX);
	wattron(stdscr, COLOR_PAIR(7));
	while(KeyPress = getch() ){
		switch(KeyPress){
		   case KEY_F(3):
		        InEdit = FALSE;
			   leaveok(stdscr, TRUE);
			   StatusBar();
			   move(LINES, COLS);
			   wrefresh(stdscr);
			   return;
		   case KEY_LEFT:
			   currX = NextX(--currX, FALSE);
			   break;
		   case KEY_RIGHT:
			   currX = NextX(++currX, TRUE);
			   break;
		   case KEY_UP:
			   if ( currY > 2 ) currY--;
			   break;
		   case KEY_DOWN:
			   if (currY < LINES - 3) currY++;
			   break;
		   case KEY_PPAGE:
			   break;
		   case KEY_NPAGE:
			   break;
		   default:
			   InChar = (char)KeyPress;
			   ByteAddr = DisplayStartAddr + ((currY - 2) * 16);
			   j = currX - 12;
               //Chekc position of cursor -- is it in hex bytes?
               if(currX >12 && currX < 60){
			     if ((InChar >= '0' && InChar <= '9')
		  	           || (InChar >= 'A' && InChar <= 'F')){
				     addch(InChar);
			     }
			     else if (InChar >= 'a' && InChar <= 'f'){
				     InChar -= 32;
				     addch( InChar);
		   	     }
			     else break;

			     ByteAddr += j / 3;
			     if (j % 3 == 2) mvinnstr( currY, currX -1, Buf, 2);
			     else mvinnstr( currY, currX, Buf, 2);
			     NewByte = xatoi(Buf[0]) * 16;
			     NewByte += xatoi(Buf[1]);

			     MappedFile[ByteAddr] = NewByte;
			     // update ASCII
			     j = (currX - 13 )/ 3 + (COLS-16);
			     if (NewByte >= 32 && NewByte <= 254 )
			         mvwprintw( stdscr, currY, j, "%c", NewByte);
			     else mvwprintw(stdscr, currY, j, ".");
			     break;
			   }
			   else {
			       addch( InChar);
			       ByteAddr += currX - ( COLS - 16);
			       MappedFile[ByteAddr] = InChar;
			       //update HEX
			       j = ((currX - (COLS - 16)) * 3 ) + 13;
			       mvwprintw(stdscr, currY, j, "%.2X", InChar);
			       break;
			    }
		}
		move(currY, currX);
        wrefresh(stdscr);
	}
	return;
}


//-----------------------main window display stuff
void StatusBar(){
	char cright[] = "code 1999 per mammon_";
	wattron(stdscr, COLOR_PAIR(1));
	BlankLine(0);
	mvprintw(0,0,"Linux Hex Editor");
	mvprintw(0, COLS - strlen(cright), "%s", cright);
	wattron(stdscr, COLOR_PAIR(6));
	BlankLine(1);
	mvprintw(1,0,"File: %s", fname);
	move(1, COLS / 2 - 5);
	if (InEdit) {
		wattron(stdscr, COLOR_PAIR(5));
		printw("Edit Mode ON ");
	}
	else printw("Edit Mode OFF");
	move(LINES, COLS);
	wrefresh(stdscr);
	return;
}

void FKeyLabels(){
	int i;
	char FKLabels[10][6] = { {" Help "}, {" Save "}, {" Edit "}, {"      "}, {"      "}, {" Goto "}, {" Find "}, {"  Hdr "}, {"      "}, {" Quit "} };
	char FK[10][3] ={ {"F1"}, {"F2"}, {"F3"}, {"F4"}, {"F5"}, {"F6"}, {"F7"}, {"F8"}, {"F9"}, {"F0"} };
	wattron(stdscr, COLOR_PAIR(2));
	BlankLine( LINES - 1);
	BlankLine( LINES - 2);
	wattron(stdscr, COLOR_PAIR(6));
	for (i = 0; i < 10; i++) {
		move( LINES - 2, (i * 8) + 1);
		printw("  %s  ", FK[i]);
		move( LINES - 1, (i * 8) + 1);
		printw("%.6s",FKLabels[i] );
	}
	return;
}


void DisplayPage(){
	char ReadBuf[16], HexBuf[2];
	int x, y, pos;
	off_t addr;

	addr = DisplayStartAddr;
	for (y = 2; y < LINES - 2 ; y++) {
		wattron(stdscr, COLOR_PAIR(4));
		BlankLine(y);

		//print the  address
		mvprintw( y, 0, "%08X    ", addr);

		//print the ASCII Bytes
		wattron(stdscr, COLOR_PAIR(3) );
		for (x = 0; x < 16; x++) {
			move(y, (COLS - 16) + x);
			if ((addr + x) >= FileSize)
				printw(" ");
			else if (MappedFile[addr + x] <= 32 ||
				 MappedFile[addr + x] == 255) printw(".");
			else printw("%c", MappedFile[addr + x]);
		}
		//print the Hex Bytes
		wattron(stdscr, COLOR_PAIR(2));
		for (x = 0; x < 16; x++) {
			move(y, 12 + (x * 3) );
			if ((addr + x) >= FileSize)
				sprintf(HexBuf, "     ");
			else sprintf(HexBuf, "%.4hX", MappedFile[addr +x]);
			printw(" %c%c", *(HexBuf + 2), *(HexBuf + 3));
		}
		addr += 16;
	}
	move(LINES, COLS );			//hide cursos
	wrefresh(stdscr);			//refresh window
	return;
}

bool MenuHandler(int KeyPress){
	switch (KeyPress) {
        case KEY_HOME:
			DisplayStartAddr = 0;
			StatusBar();
			DisplayPage();
			break;
		case KEY_END:
			DisplayStartAddr = LastPage;
			StatusBar();
			DisplayPage();
			break;
		case KEY_UP:
			if (DisplayStartAddr > 0 )
				DisplayStartAddr -= 16;
			StatusBar();
			DisplayPage();
			break;
		case KEY_DOWN:
			if (DisplayStartAddr + SIZEofPAGE < FileSize)
				DisplayStartAddr += 16;
			StatusBar();
			DisplayPage();
			break;
		case KEY_NPAGE:
			if ( DisplayStartAddr + (16 * (LINES - 4)) +
					(16 * (LINES - 4)) >= FileSize)
				DisplayStartAddr = FileSize - (SIZEofPAGE - 16) - (FileSize % 16);
			else DisplayStartAddr += 16 * (LINES - 5);
			StatusBar();
			DisplayPage();
			break;
		case KEY_PPAGE:
			if (DisplayStartAddr >= SIZEofPAGE )
				DisplayStartAddr -= 16 * (LINES - 5);
			else DisplayStartAddr = 0;
			StatusBar();
			DisplayPage();
			break;
		case KEY_F(1):
			HelpWindow();
			break;
		case KEY_F(2):
			SaveFile();
			break;
		case KEY_F(3):
			EditWindow();
			break;
		case KEY_F(4):
			//mode;
			break;
		case KEY_F(5):
			//??
			break;
		case KEY_F(6):
			Goto();
			break;
		case KEY_F(7):
			Search();
			break;
		case KEY_F(8):
			ParseELFHeader();
			break;
		case KEY_F(9):
			//truncate
			break;
        case KEY_F(10):
            return FALSE;
		default:
			break;
		}
	return TRUE;
}






int main(int argc, char *argv[]) {
	//Check arguments
	if (argc != 2) {
		printf("Usage: %s [filename]\n\n", argv[0]);
		exit(1);
	}

	//-----Start NCurses Stuff -----------------------------
	initscr();
	noecho();
	SetupColors();
	leaveok(stdscr, TRUE);
	wclear(stdscr);

	fname = argv[1];
	LoadFile();

	//print stat bar and menu and first page of file
	FKeyLabels();
	StatusBar();
	DisplayStartAddr = 0;
	DisplayPage();

	//enter event loop
	keypad(stdscr, TRUE);
	while( MenuHandler(getch()) ) ;

	munmap( MappedFile, FileSize);
	close(fd);
	echo();					//turn echo back on
	endwin();				//turn off ncurses
	return 0;
}

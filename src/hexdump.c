//hexdump.c  simple ncurses program
//compile with
//gcc -o hexdump hexdump.c -lncurses
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ncurses/curses.h>

int fd, addr, pos;

void dump_page(){
	char buf[16], buf2[4];
	int x, y;
	for (y = 1; y < LINES -1 ; y++) {
		if ((pos = read(fd, buf, sizeof(buf)) ) == 0) {
		        move(y,0);
			printw("                                                                                ");
			continue;
		}
		wattron(stdscr, COLOR_PAIR(2));		//bright addr colors
		move(y,0);
		printw("%08X    ", addr);		//like printf
		wattron(stdscr, COLOR_PAIR(1));		//neato hexdump colors
		for (x = 0; x < 16; x++){
			move(y, 12 + (x * 3));
			if (x >= pos) sprintf(buf2,  "    ");
			else  sprintf(buf2, "%.4hX", buf[x]);
            printw(" %c%c", *(buf2 +2), *(buf2 + 3) );  //overcome printf's deficiencies
		}
		printw("    ");
		wattron(stdscr, COLOR_PAIR(3));		//more cool colors
		for (x = 0; x < 16; x++){
			move(y, 64 + x);
			if (x >= pos) printw(" ");
			else if (buf[x] <= 32 || buf[x] == 255)  printw(".");
			else printw("%c", buf[x]);
		}
		addr += 16;
	}
	move(LINES - 1, 80);				//relocate cursor
	wrefresh(stdscr);				//repaint screen
}

int main(int argc, char *argv[]) {
	pos = 1; addr = 0;
	if (argc != 2) {
		printf("Usage: hexdump [filename]\n");
		exit(1);
	}
	if ((fd = open( argv[1], O_RDONLY, NULL)) < 0 ) {
		perror("open");
		exit(1);
	}

	initscr();				//start ncurses
	noecho();				//kill char echo
	wclear(stdscr);
	if( has_colors() ) {
		start_color();			//setup color-pairs
		init_pair(1, COLOR_CYAN, COLOR_BLUE);
		init_pair(2, COLOR_WHITE, COLOR_BLUE);
		init_pair(3, COLOR_GREEN, COLOR_BLUE);
	}

	move(0, 0);				//set screen pos [top]
	wattron(stdscr, COLOR_PAIR(2));		//change color
	printw("Address      Binary Dump                                        ASCII           ");				//print Header
	move(LINES - 1, 0);			//set screen pos [botton]
	printw("Press any key for the next page or 'q' to quit...                               ");				//print Footer

	while( pos != 0 && ( addr == 0 || getch()!= 'q' )  ) dump_page();

	move(LINES - 1, 0);
	wattron(stdscr, COLOR_PAIR(2));
	printw("End of output, press any key to continue...           ");
	wrefresh(stdscr);			//refresh window
	getch();				//ncurses getchar()
	close(fd);
	echo();					//turn echo back on
	endwin();				//turn off ncurses
	return 0;
}

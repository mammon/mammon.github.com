#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

int KeepRunning;

int GetCmd(){
	char c, c2, c3;
	char p1[3] = ";>";
	char cmdBuf[128]={0x0, 0x0};
	char cESC[2]={0x1B, 0x0};
	int pos = 0;
	int EndOfLine = 0;
	int retVal = 0;
	struct termios old_term;
	struct termios new_term;
	//struct cmdhist cur;
	//cur = head

	/* do this only if at a terminal !!! */
	/* e.g. provide two parsers :) */
	if (tcgetattr(0, &old_term) < 0) return(-1);
	new_term = old_term;
	new_term.c_lflag &= ~(ICANON | ECHO);
	new_term.c_cc[VMIN] = 1;
	new_term.c_cc[VTIME] = 0;

	write(0, "\n", 1);
	write(0, p1, 2);
	
	/* Get Command Line -- replaces gets() */
	//tty raw
	if (tcsetattr(0, TCSADRAIN, &new_term) < 0) return(-1);
	while (! EndOfLine) {
		read(0, &c, 1);
		//printf("%d %c\n", c, c & 255, c);
		switch (c) {
			//case 0x1b:
			case 0x27:
				/* handle '[' sequences */
				write(1, " 27", 3);
				read(0, &c2, 1);
				if (c2 == 0x58){
					read(0, &c3, 1);
					write(1, " 58", 3);
					if (c3 == 0x44) write(1, "lAr", 3);
				}
				break;
			case 91:
				/* handle '[' sequences */
				read(0, &c2, 1);
				if (c2 == 0x58){
					read(0, &c3, 1);
					if (c3 == 0x44) printf("lAr");
				}
				switch (c2) {
					case 65:	// UpArrow
						//cur = cur.next
						//printf cur.str
						 break;
					case 66:	// DownArrow
						//cur = cur.prev
						//printf cur.str
						break;
					case 67: //Right Arrow;
						pos++;
						write(1, "rAr", 3);
						write(1, cESC, 1);
						write(1, "[C1", 3);
						break;
					case 68: //Left Arrow;
						pos--;
						printf("lAr");
						write(1, cESC, 1);
						write(1, "[2D", 3);
						break;
					default:
						write(0, &c, 1);
						write(0, &c2, 1);
				}
				break;
			case 27:
				/* handle control key combos */
				read(0, &c2, 1);
				switch (c2) {
					case 91:
						/* control-[ combos */
						read(0, &c3, 1);
						switch (c3){
							case 65:	// Ctrl-UpArrow
								// ??
								break;
							case 66:	// Ctrl-DownArrow
								// ??
								break;
							case 67:	// Ctrl-RightArrow
								// forward 1 word
								break;
							case 68:	// Ctrl-LeftArrow
								// back one word
								break;
						}
						break;
					case 'h':
						write(1, "\nCommand Hist\n", 14);
					case '\b':
					case 127:
						// Clear Line
						write(1, "\nClear Line\n", 12);
						break;
					case '/':
						// REPLACE
						write(1, "\nsubstitute\n", 12);
						break;
					default:
						// nop	
				}
				break;
			case '\b':
			case 127:
				//write(0, "<BREAK>", 7);
				write(1, "\b \b", 3);
				cmdBuf[strlen(cmdBuf) -1] = '\0';
				break;
			case '\n':
			case '\r':
				EndOfLine=1; 
				/* add to cmd history buffer by free()ing head.str and
				 * malloc()ing space for a new string.
				 */
			default:
				write(1, &c, 1);
				cmdBuf[pos] = c;
				pos++;
				// removes this and add to reg processing later
				if (c == 'q'){ retVal = 1; EndOfLine = 1;}
				//sprintf(cmdBuf, "%s%c", cmdBuf, c);
		}
	}

	/*  Process Command Line */
	//ttyreset
	if (tcsetattr(0, 0, &old_term) < 0) return(-1);
	printf("DEBUG>> Command Line: %s\n", cmdBuf);

	return(retVal);
}

int main(){
	int retVal = 0;
	while (retVal == 0){
		retVal = GetCmd();
	}
	return(0);
}

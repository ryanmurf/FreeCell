/*  This file contains C code that can be used for the freecell
 programming problem in ECEn 521. This is a modified version
 of code I developed for my own solution to the problem.

 This code contains routines and data structures to read input,
 initialize a compact representation of the game state, display a
 given state, test for a solution, etc. (It assumes that the input
 comes from stdin; if "freecell" is the executable, you type
 "freecell < game1.txt" to run the program on that test case.)

 I don't want anything in this code to constrain your solution.
 Feel free to use or modify any portion of this code as you see
 fit.  You might, for example, create a data structure that
 consists of the sequence of states reachable from the initial state.
 */

#include "freecell.h"

//Special print Characters for shapes
char Spade[4] = {0xE2,0x99,0xA0,0};
char Heart[4] = {0xE2,0x99,0xA1,0};
char Diamond[4] = {0xE2,0x99,0xA2,0};
char Club[4] = {0xE2,0x99,0xA3,0};

State initial; /* the starting state, loaded from stdin */
Card deck[DECKSIZE]; /* card values in program are indices to deck[] */

int main(int argc, char *argv[]) {
	int i;
	States *k;
	initdeck();
	readinitconfig();

	printf("\n");
	printf("Created state 0\n");
	printf("Size of State %u\n", (int) sizeof(State));
	initial.score = 0;
	dumpstate(&initial);

	//add_state(table, &initial);
	//array = lookup_state(table, &initial);
	printf("Possible Moves : %i\n", possibleMoves(&initial));

	search();

	//k = generateNextStates(&initial);

	//for(i=0; i<k->size; i++) {
	//	dumpstate(k->states[i]);
	//}

	return 0;
}

void initdeck(void) {
	/* initialize all card values in deck */

	int i, j;
	i = 0;
	for (j = 0; j < SUITSIZE; j++, i++) {
		deck[i].num = j + 1;
		deck[i].suit = DIAMONDS;
		deck[i].color = RED;
		deck[i].played = 0;
	}
	for (j = 0; j < SUITSIZE; j++, i++) {
		deck[i].num = j + 1;
		deck[i].suit = HEARTS;
		deck[i].color = RED;
		deck[i].played = 0;
	}
	for (j = 0; j < SUITSIZE; j++, i++) {
		deck[i].num = j + 1;
		deck[i].suit = SPADES;
		deck[i].color = BLACK;
		deck[i].played = 0;
	}
	for (j = 0; j < SUITSIZE; j++, i++) {
		deck[i].num = j + 1;
		deck[i].suit = CLUBS;
		deck[i].color = BLACK;
		deck[i].played = 0;
	}
}

int getinputline(char *s, int n) { /* reads line from stdin, copies up to n chars to s, returns
 number of chars written */
	int i, c;
	i = 1;
	while ((c = getchar()) != '\n') {
		if (i <= n)
			*s++ = c;
		i++;
	}
	*s = '\0';
	return ((i > n) ? n : i);
}

int mapnum(char c) {
	/* takes single char card identifier from input and gives the card
	 number used throughout the program */

	switch (c) {
	case 'A':
		return 1;
	case '2':
		return 2;
	case '3':
		return 3;
	case '4':
		return 4;
	case '5':
		return 5;
	case '6':
		return 6;
	case '7':
		return 7;
	case '8':
		return 8;
	case '9':
		return 9;
	case 'T':
		return 10;
	case 'J':
		return 11;
	case 'Q':
		return 12;
	case 'K':
		return 13;
	default:
		printf("Bad value in mapnum\n");
		exit(-1);
	}
}

int mapsuit(char c) {
	/* takes single char suit identifier and returns the appropriate
	 enumerated type */

	switch (c) {
	case 'D':
		return DIAMONDS;
	case 'H':
		return HEARTS;
	case 'S':
		return SPADES;
	case 'C':
		return CLUBS;
	default:
		printf("Bad value in mapsuit\n");
		exit(-1);
	}
}

int getindex(int num, int suit) {
	/* returns index to deck for card of this num and suit */

	int base;
	switch (suit) {
	case DIAMONDS:
		base = 0;
		break;
	case HEARTS:
		base = SUITSIZE;
		break;
	case SPADES:
		base = SUITSIZE * 2;
		break;
	case CLUBS:
		base = SUITSIZE * 3;
		break;
	}
	return base + num - 1;
}

void readinitconfig(void) {
	/* input assumed to come via stdin */

	int i, j, k, num, suit, len, index;
	char buf[BUFSIZE];

	/* first, clear initial state */
	for (i = 0; i < NUMCOLS; i++) {
		for (j = 0; j < COLSIZE; j++)
			initial.column[i][j] = (char) -1;
		initial.colheight[i] = 0;
	}
	for (i = 0; i < CELLS; i++)
		initial.freecell[i] = (char) -1;
	for (i = 0; i < CELLS; i++)
		initial.stack[i] = (char) -1;

	/* read one line of input per column */
	for (i = 0; i < NUMCOLS; i++) {
		len = getinputline(buf, BUFSIZE - 1);
		j = 0;  // index to buffer
		k = 0;  // index to column slot
		while (j < len) {
			num = mapnum(buf[j]);
			suit = mapsuit(buf[j + 1]);
			index = getindex(num, suit);
			if (deck[index].played) {
				printf("Error: card repeated (%c%c)\n", buf[j], buf[j + 1]);
				exit(-1);
			}
			deck[index].played = 1;
			initial.column[i][k++] = index;
			initial.colheight[i]++;
			j += 3;
		}
	}

	/* there should be no repeated cards at this point, but make sure
	 that there are no missing cards */
	for (i = 0; i < DECKSIZE; i++) {
		if (deck[i].played == 0) {
			printf("Error: card (index: %d) missing from deck\n", i);
			exit(-1);
		}
	}

	printf("Input game verified and loaded\n");
}

void dumpcard(char index) {
	/* output a single card in the specified output format, add space
	 * at the end */


	if (index < 0 || index >= DECKSIZE) {
		if (index == -1) {
			printf(".. ");
			return;
		}
		printf("Bad value in dumpcard %i\n", index);
		exit(-1);
	}
	printf(ANSI_COLOR_BG_WHITE);
	if(deck[(int) index].color == RED) {
		printf(ANSI_COLOR_RED);
	} else {
		printf(ANSI_COLOR_BLACK);
	}
	switch (deck[(int) index].num) {
	case 1:
		printf("A");
		break;
	case 2:
		printf("2");
		break;
	case 3:
		printf("3");
		break;
	case 4:
		printf("4");
		break;
	case 5:
		printf("5");
		break;
	case 6:
		printf("6");
		break;
	case 7:
		printf("7");
		break;
	case 8:
		printf("8");
		break;
	case 9:
		printf("9");
		break;
	case 10:
		printf("T");
		break;
	case 11:
		printf("J");
		break;
	case 12:
		printf("Q");
		break;
	case 13:
		printf("K");
		break;
	}
	switch (deck[(int) index].suit) {
	case DIAMONDS:
		printf("%s",Diamond);
		break;
	case HEARTS:
		printf("%s",Heart);
		break;
	case SPADES:
		printf("%s", Spade);
		break;
	case CLUBS:
		printf("%s", Club);
		break;
	}
	printf(" ");
	printf(ANSI_COLOR_RESET);
}

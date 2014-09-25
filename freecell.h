/*
 * freecell.h
 *
 *  Created on: Sep 25, 2014
 *      Author: ryan
 */

#ifndef FREECELL_H_
#define FREECELL_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define DECKSIZE 52
#define SUITSIZE 13
#define NUMCOLS   8
#define COLSIZE  20    /* can't have more than 19 cards in a column */
#define CELLS     4    /* both freecells and stacks */
#define BUFSIZE  80


#define ANSI_COLOR_BG_WHITE "\x1b[47m"
#define ANSI_COLOR_BLACK   "\x1b[30m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define MAX_DEPTH 4

enum suits {
	DIAMONDS, HEARTS, SPADES, CLUBS
};
enum colors {
	RED, BLACK
};

typedef struct cardrec /* one of these per card */
{
	int num; /* values 1-13, for A through K */
	int suit; /* one of enum suits */
	int color; /* one of enum colors */
	int played; /* flag: test that all cards are present */
} Card;

typedef struct State State;

struct State /* the state of the game at each possible move */
{
	char column[NUMCOLS][COLSIZE]; /* 8 columns of up to 20 cards each */
	char freecell[CELLS]; /* 4 freecells */
	char stack[CELLS]; /* 4 stacks: stores top card only */
	char colheight[NUMCOLS]; /* this and previous 2 used for hashing */
	int status; /* 1: in history, 2: exhausted, otherwise 0 */
	State *children; //Points to an array of children of the possible moves of this state
};

//Functions to work with state
void dumpstate(State *x);
int endstate(State *s);
int checkStack(const State *state, int card);
int checkCardToColumn(const State *state, int card, int column);
int checkFreeCell(const State *state, int card);
int possibleMoves(const State * state);
int genMoveStates(State * state, int depth);


//
void initdeck(void);
int getinputline(char *s, int n);
int mapnum(char c);
int mapsuit(char c);
int getindex(int num, int suit);
void readinitconfig(void);
void dumpcard(char index);




#endif /* FREECELL_H_ */

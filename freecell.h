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
#include <stdbool.h>

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

#define MAX_DEPTH 5
#define GLOBAL_HASH_SIZE 10000
#define PATH_HASH_SIZE 10000

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
    const char** path;
    int p_size;
};

typedef struct States {
    State** states;
    int size;
} States;

typedef struct _list_t_ {
	State *state;
	struct _list_t_ *next;
} list_t;

typedef struct _hash_table_t_ {
	int size;
	list_t **table;
} hash_table_t;

//Functions to work with state
void dumpstate(State *x);
int endstate(State *s);
int checkStack(const State *state, int card);
int checkCardToColumn(const State *state, int card, int column);
int checkFreeCell(const State *state, int card);
int possibleMoves(const State * state);
int genMoveStates(State * state, int depth);
int sameState(State *s1, State *s2);
State* subtreeSearch(State* state, hash_table_t* hashTable, int depth);
//Search needs input to build initial state
State* search();
//HashCheck should return all the states that are not contained in the 
//hash table. It should also update the hash table with all of the new
//states.
States* hashCheck(States* states, hash_table_t hashTable);
//GenerateNextStates should produce all of the states that can be reached
//from s in one move. 
States* generateNextStates(State* s);
//HashCheckSingle should determine if s is contained within the hash table.
//It should also update the hash table accordingly.
bool hashCheckSingle(State* s, hast_table_t hashTable);
int scoreState(State* s, int (*scoringFunc)(State*));

//
void initdeck(void);
int getinputline(char *s, int n);
int mapnum(char c);
int mapsuit(char c);
int getindex(int num, int suit);
void readinitconfig(void);
void dumpcard(char index);

//
hash_table_t *create_hash_table(int size);
unsigned int hash(hash_table_t *hashtable, State *s);
State *lookup_state(hash_table_t *hashtable, State *s);
int add_state(hash_table_t *hashtable, State *s);
void free_table(hash_table_t *hashtable);


#endif /* FREECELL_H_ */

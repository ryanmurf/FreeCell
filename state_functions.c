/*
 * state_functions.c
 *
 *  Created on: Sep 25, 2014
 *      Author: ryan
 */

#include "freecell.h"

extern Card deck[DECKSIZE];

void dumpstate(State *x) {
	int i, j, flag;

	/* generate output that mimics actual game display, with freecells
	 on the first row with stacks, then each row of the columns as
	 far as they go */

	for (i = 0; i < 23; i++)
		printf("=");
	printf("\n");
	for (i = 0; i < CELLS; i++)
		dumpcard(x->freecell[i]);
	for (i = 0; i < CELLS; i++)
		dumpcard(x->stack[i]);
	printf("\n");
	for (i = 0; i < 23; i++)
		printf("-");
	printf("\n");

	for (i = 0; i < COLSIZE; i++) {
		/* do another row of output */
		flag = 0; /* set flag if row non-empty */
		for (j = 0; j < NUMCOLS; j++) {
			if (i < x->colheight[j])
				dumpcard(x->column[j][i]);
			else
				printf("   ");
			if (i < x->colheight[j] - 1)
				flag = 1;
		}
		printf("\n");
		if (!flag)
			break;
	}
	for (i = 0; i < NUMCOLS; i++) {
		printf("%2d ", x->colheight[i]);
		assert(x->colheight[i] >= 0);
	}

	printf("\n");
	for (i = 0; i < 23; i++)
		printf("=");
	printf("\n");
}

int endstate(State *s) {
	/* return 1 if s is winning state, else 0 */
	int i;
	for (i = 0; i < NUMCOLS; i++) {
		if (s->colheight[i] != 0)
			return 0;
	}
	for (i = 0; i < CELLS; i++) {
		if (s->freecell[i] != -1)
			return 0;
	}
	if ((s->stack[0] != 12) || (s->stack[1] != 25) || (s->stack[2] != 38)
			|| (s->stack[3] != 51)) {
		printf("Impossible condition in endstate\n");
		exit(-1);
	}
	return 1;
}

//Return 1-4 if card can move to stack. If not -1
int checkStack(const State *state, int card) {
	int i;
	State s = *state;
	for (i = 0; i < CELLS; i++) {
		if (s.stack[i] == -1 && deck[card].num == 1) //if empty and have ace
			return i+1;
		else {
			if (deck[card].color == deck[(int)s.stack[i]].color) {
				if (deck[card].suit == deck[(int)s.stack[i]].suit) {
					if (deck[card].num == (deck[(int)s.stack[i]].num + 1)) {
						return i+1;
					}
				}
			}
		}
	}
	return 0;
}

int checkCardToColumn(const State *state, int card, int column) {
	int columnCard;
	//no cards in column
	if(state->colheight[column] == 0) {
		return 1;
	}
	columnCard = state->column[column][state->colheight[column]-1];
	if(card == columnCard)
		return 0;
	if (deck[columnCard].color != deck[card].color) {
		if((deck[columnCard].num-1) == deck[card].num) {
			return 1;
		}
	}
	return 0;//false
}

int checkFreeCell(const State *state, int card) {
	int i;

	for (i = 0; i < CELLS; i++) {
		if(state->freecell[i] == -1)
			return i+1;
	}
	return 0;
}

int possibleMoves(const State * state) {
	int i,j;
	int card = 0;
	int moves = 0;
	State s = *state;

	//Start with FreeCells
	for (i = 0; i < CELLS; i++) {
		if (s.freecell[i] != -1) {
			//Check to see if we can move to Stack
			if (checkStack(state, s.freecell[i]))
				moves++;
			//Check 8 columns
			for(j=0; j<NUMCOLS; ++j) {
				if(checkCardToColumn(state, s.freecell[i], j))
					moves++;
			}
		}
	}
	//Now Columns
	for(i=0; i<NUMCOLS; ++i) {
		card = s.column[i][s.colheight[i]-1];
		//printf("Column %i Card %d Height %d- ", i, card, s.colheight[i]);
		if(checkFreeCell(state, card)) {
			//dumpcard(card);
			//printf("FC ");
			moves++;
		}
		if(checkStack(state,card)) {
			//dumpcard(card);
			//printf("ST ");
			moves++;
		}
		for(j=0; j<NUMCOLS; ++j) {
			if(checkCardToColumn(state, card, j)) {
				//dumpcard(card);
				//printf("C%i ",j+1);
				moves++;
			}
		}
		//printf("\n");
	}

	return moves;
}


int genMoveStates(State * state, int depth) {
	//printf("\n");
	State s = *state;
	int i,j,posMoves,spot,card, sum=0;
	int max=0, location=0;
	posMoves = possibleMoves(&s);

	if(posMoves == 0 || depth > MAX_DEPTH) {
		//printf("No Moves Left.\n");
		return 0;
	}

	State *temp = malloc(sizeof(State) * posMoves);
	//Copy the parent state to each of the children since the difference will be small
	for(i=0; i<posMoves; i++) {
		memcpy(&temp[i], state, sizeof(State));
	}

	//Use this to keep track of sates in array
	posMoves = 0;

	//Start with FreeCells
	for (i = 0; i < CELLS; i++) {
		if (s.freecell[i] != -1) {
			//Check to see if we can move to Stack
			spot = checkStack(state, s.freecell[i]);
			if (spot) {
				//Move the card
				temp[posMoves].stack[spot-1] = temp[posMoves].freecell[i];
				//Clear the old spot;
				temp[posMoves].freecell[i] = -1;
				//add to its score
				temp[posMoves].status += 10;
				//Move to the next possible move
				posMoves++;
			}
			//Check 8 columns
			for(j=0; j<NUMCOLS; ++j) {
				spot = checkCardToColumn(state, s.freecell[i], j);
				if (spot) {
					//Move the card
					temp[posMoves].column[j][(int)temp[posMoves].colheight[j]] = temp[posMoves].freecell[i];
					//Clear the old spot;
					temp[posMoves].freecell[i] = -1;
					//increase counter
					temp[posMoves].colheight[j] ++;
					//add to its score
					temp[posMoves].status += 0;
					//Move to the next possible move
					posMoves++;
				}
			}
		}
	}

	//Now Columns
	for (i = 0; i < NUMCOLS; ++i) {
		card = s.column[i][s.colheight[i] - 1];
		spot = checkFreeCell(state, card);
		if (spot) {
			//Move the card
			temp[posMoves].freecell[spot-1] = temp[posMoves].column[i][temp[posMoves].colheight[i] - 1];
			//Clear the old spot;
			temp[posMoves].column[i][temp[posMoves].colheight[i] - 1] = -1;
			//Reduce the column height
			temp[posMoves].colheight[i]--;
			//add to its score
			temp[posMoves].status += 0;
			//Move to the next possible move
			posMoves++;
		}
		spot = checkStack(state, card);
		if (spot) {
			//Move the card
			temp[posMoves].stack[spot-1] = temp[posMoves].column[i][temp[posMoves].colheight[i] - 1];
			//Clear the old spot;
			temp[posMoves].column[i][temp[posMoves].colheight[i] - 1] = -1;
			//Reduce the column height
			temp[posMoves].colheight[i]--;
			//add to its score
			temp[posMoves].status += 10;
			//Move to the next possible move
			posMoves++;
		}
		for (j = 0; j < NUMCOLS; ++j) {
			spot = checkCardToColumn(state, card, j);
			if (spot) {
				//Move the card
				temp[posMoves].column[j][(int)temp[posMoves].colheight[j]] = temp[posMoves].column[i][(int)temp[posMoves].colheight[i] - 1];
				//Clear the old spot;
				temp[posMoves].column[i][temp[posMoves].colheight[i] - 1] = -1;
				//Reduce the column height for i
				temp[posMoves].colheight[i]--;
				//Increase column height for j
				temp[posMoves].colheight[j]++;
				//add to its score
				temp[posMoves].status += 0;
				//Move to the next possible move
				posMoves++;
			}
		}
	}

	for(i=0; i<posMoves; i++) {
		if(endstate(&temp[i]))
			dumpstate(&temp[i]);
		temp[i].status += genMoveStates(&temp[i], depth+1);
	}
	max = temp[0].status;
	for(i=0; i<posMoves; i++) {
		sum += temp[i].status;
		if(temp[i].status > max) {
			max = temp[i].status;
			location = i;
		}
	}
	if(max == 0) {
		state->children = NULL;
		free(temp);
	} else {
		state->children = malloc(sizeof(State));
		memcpy(state->children, &temp[location], sizeof(State));
		free(temp);
	}

	return sum;
}

int sameState(State *s1, State *s2) {
	int i,j;

	for(i=0; i<CELLS; i++) {
		if(s1->freecell[i] != s2->freecell[i]) {
			return 0;
		}
		if(s1->stack[i] != s2->stack[i]) {
			return 0;
		}
	}
	for(i=0; i<COLSIZE; i++) {
		for(j=0; j<s1->colheight[i]; j++) {
			if(j != s2->colheight[i])
				return 0;
			if(s1->column[i][j] != s2->column[i][j])
				return 0;
		}
	}
	return 1;
}


/*
 * state_functions.c
 *
 *  Created on: Sep 25, 2014
 *      Author: ryan
 */

#include "freecell.h"

extern State initial;
extern Card deck[DECKSIZE];

int simpleScore(State *s) {

	return 0;
}

State* subtreeSearch(State* s, hash_table_t* hashTable, int depth) {
    if (depth == MAX_DEPTH) return s;
    States* states = generateNextStates(s);
    States* validStates = hashCheck(states, hashTable);
    State* bestState = s;
    int i, bestScore = scoreState(s, simpleScore);
    for (i = 0; i < validStates->size; i++) {
        State* temp = subtreeSearch(validStates->states[i], hashTable, depth+1);
        int score = scoreState(temp, simpleScore);
        if (score > bestScore) {
            free(bestState);
            bestScore = score;
            bestState = temp;
        } else {
            free(temp);
        }
    }
    free(states);
    free(validStates);
    return bestState;
}

//The input to this function will need to be the config for a start state
State* search() {
    State *s;
    hash_table_t *path_hash;
	hash_table_t *global_hash = create_hash_table(PATH_HASH_SIZE);
    //Build start state -- using config
    State* start = &initial;
    do {
        path_hash = create_hash_table(GLOBAL_HASH_SIZE);
        s = subtreeSearch(start, path_hash, 0);
        if (endstate(s) == true) {
            //printPath(s);
            exit(0);
        }
        free_table(path_hash);
    } while (hashCheckSingle(s, global_hash) == false);
    printf("No solution found, exiting...\n");
    exit(0);
}

int scoreState(State* s, int (*scoringFunction)(State*)) {
    return scoringFunction(s);
}
    
bool hashCheckSingle(State* s, hash_table_t* hashTable) {
    if (add_state(hashTable, s) == 0) {
        return false;
    } else {
        return true;
    }    
}

States* hashCheck(States* states, hash_table_t* hashTable) {
    int count = 0, i;
    for (i = 0; i < states->size; i++ ) { 
        if (lookup_state(hashTable, states->states[i]) == NULL) {
            count++;
        }
    }
    States* validStates = (States*) malloc(sizeof(States));
    validStates->states = (State**) calloc(sizeof(State*), count);
    
    int j = 0;
    for (i = 0; i < states->size; i++) {
        if (add_state(hashTable, states->states[i]) == 0) {
            (validStates->states)[j] = states->states[i];
            j++;
        }
    }
    validStates->size = j;
    return validStates;
}

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
			return i + 1;
		else {
			if (deck[card].color == deck[(int) s.stack[i]].color) {
				if (deck[card].suit == deck[(int) s.stack[i]].suit) {
					if (deck[card].num == (deck[(int) s.stack[i]].num + 1)) {
						return i + 1;
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
	if (state->colheight[column] == 0) {
		return 1;
	}
	columnCard = state->column[column][state->colheight[column] - 1];
	if (card == columnCard)
		return 0;
	if (deck[columnCard].color != deck[card].color) {
		if ((deck[columnCard].num - 1) == deck[card].num) {
			return 1;
		}
	}
	return 0; //false
}

int checkFreeCell(const State *state, int card) {
	int i;

	for (i = 0; i < CELLS; i++) {
		if (state->freecell[i] == -1)
			return i + 1;
	}
	return 0;
}

int possibleMoves(const State * state) {
	int i, j;
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
			for (j = 0; j < NUMCOLS; ++j) {
				if (checkCardToColumn(state, s.freecell[i], j))
					moves++;
			}
		}
	}
	//Now Columns
	for (i = 0; i < NUMCOLS; ++i) {
		card = s.column[i][s.colheight[i] - 1];
		//printf("Column %i Card %d Height %d- ", i, card, s.colheight[i]);
		if (checkFreeCell(state, card)) {
			//dumpcard(card);
			//printf("FC ");
			moves++;
		}
		if (checkStack(state, card)) {
			//dumpcard(card);
			//printf("ST ");
			moves++;
		}
		for (j = 0; j < NUMCOLS; ++j) {
			if (checkCardToColumn(state, card, j)) {
				//dumpcard(card);
				//printf("C%i ",j+1);
				moves++;
			}
		}
		//printf("\n");
	}

	return moves;
}

States* generateNextStates(State* s) {
	int i, j, posMoves, spot, card;
	States *states = malloc(sizeof(States));

	states->size = possibleMoves(s);

	if (states->size == 0) {
		states->states = NULL;
		return states;
	}

	states->states = (State**) malloc(sizeof(State*) * states->size);
	for(i=0; i<states->size; i++) {
		(states->states)[i] = malloc(sizeof(State));
	}
	//Copy the parent state to each of the children since the difference will be small
	for (i = 0; i < states->size; i++) {
		memcpy((states->states)[i], s, sizeof(State));
	}

	posMoves = 0;
	//Start with FreeCells
	for (i = 0; i < CELLS; i++) {
		if (s->freecell[i] != -1) {
			//Check to see if we can move to Stack
			spot = checkStack(s, s->freecell[i]);
			if (spot) {
				//Move the card
				(states->states)[posMoves]->stack[spot - 1] = (states->states)[posMoves]->freecell[i];
				//Clear the old spot;
				(states->states)[posMoves]->freecell[i] = -1;
				//add to its score
				//(states->states)[posMoves]->status += 10;
				//Move to the next possible move
				posMoves++;
			}
			//Check 8 columns
			for (j = 0; j < NUMCOLS; ++j) {
				spot = checkCardToColumn(s, s->freecell[i], j);
				if (spot) {
					//Move the card
					(states->states)[posMoves]->column[j][(int) (states->states)[posMoves]->colheight[j]] = (states->states)[posMoves]->freecell[i];
					//Clear the old spot;
					(states->states)[posMoves]->freecell[i] = -1;
					//increase counter
					(states->states)[posMoves]->colheight[j]++;
					//add to its score
					//(states->states)[posMoves]->status += 0;
					//Move to the next possible move
					posMoves++;
				}
			}
		}
	}

	//Now Columns
	for (i = 0; i < NUMCOLS; ++i) {
		card = s->column[i][s->colheight[i] - 1];
		spot = checkFreeCell(s, card);
		if (spot) {
			//Move the card
			(states->states)[posMoves]->freecell[spot - 1] =	(states->states)[posMoves]->column[i][(states->states)[posMoves]->colheight[i] - 1];
			//Clear the old spot;
			(states->states)[posMoves]->column[i][(states->states)[posMoves]->colheight[i] - 1] = -1;
			//Reduce the column height
			(states->states)[posMoves]->colheight[i]--;
			//add to its score
			//(states->states)[posMoves]->status += 0;
			//Move to the next possible move
			posMoves++;
		}
		spot = checkStack(s, card);
		if (spot) {
			//Move the card
			(states->states)[posMoves]->stack[spot - 1] = (states->states)[posMoves]->column[i][(states->states)[posMoves]->colheight[i] - 1];
			//Clear the old spot;
			(states->states)[posMoves]->column[i][(states->states)[posMoves]->colheight[i] - 1] = -1;
			//Reduce the column height
			(states->states)[posMoves]->colheight[i]--;
			//add to its score
			//(states->states)[posMoves]->status += 10;
			//Move to the next possible move
			posMoves++;
		}
		for (j = 0; j < NUMCOLS; ++j) {
			spot = checkCardToColumn(s, card, j);
			if (spot) {
				//Move the card
				(states->states)[posMoves]->column[j][(int) (states->states)[posMoves]->colheight[j]] = (states->states)[posMoves]->column[i][(int) (states->states)[posMoves]->colheight[i] - 1];
				//Clear the old spot;
				(states->states)[posMoves]->column[i][(states->states)[posMoves]->colheight[i] - 1] = -1;
				//Reduce the column height for i
				(states->states)[posMoves]->colheight[i]--;
				//Increase column height for j
				(states->states)[posMoves]->colheight[j]++;
				//add to its score
				//(states->states)[posMoves]->status += 0;
				//Move to the next possible move
				posMoves++;
			}
		}
	}
	return states;
}

int sameState(State *s1, State *s2) {
	int i, j;

	//method 1
	//if (s1 == s2)
	//	return 1;
	//else
	//	return 0;

	//method 2
	for (i = 0; i < CELLS; i++) {
		if (s1->freecell[i] != s2->freecell[i]) {
			return 0;
		}
		if (s1->stack[i] != s2->stack[i]) {
			return 0;
		}
	}
	for (i = 0; i < COLSIZE; i++) {
		if (s1->colheight[i] != s2->colheight[i])
			return 0;
		for (j = 0; j < s1->colheight[i]; j++) {
			if (s1->column[i][j] != s2->column[i][j])
				return 0;
		}
	}
	return 1;
}


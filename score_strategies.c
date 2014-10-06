#include "freecell.h"

extern Card deck[DECKSIZE];

int simpleScoreMeng(State* s)
{
	int stackIdx, freecellIdx;
	int score = 0;
	int columnCount = DECKSIZE;
	int colIdx, numIdx;
	
	for(stackIdx = 0; stackIdx < CELLS; stackIdx++)
	{
		if(s->stack[stackIdx] != -1)
		{
			score += 10 * deck[(int) s->stack[stackIdx]].num;
			columnCount -= deck[(int) s->stack[stackIdx]].num;
		}
	}
	
	for(freecellIdx = 0; freecellIdx < CELLS; freecellIdx++)
	{
		if(s->freecell[freecellIdx] != -1)
		{
			score += 5;
			columnCount--;
		}
	}
	
	score += columnCount;
	
	for(colIdx = 0; colIdx < NUMCOLS; colIdx++)
	{
		int currentIdx = 0;
		while(1)
		{
			if(deck[(int) s->column[colIdx][currentIdx]].num == deck[(int) s->column[colIdx][currentIdx+1]].num - 1)
			{
				score ++;
			}
			currentIdx ++;
			if(s->column[colIdx][currentIdx+1] == -1)
				break;
		}
	}
	return score;
}

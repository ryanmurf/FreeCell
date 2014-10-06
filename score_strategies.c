#include "freecell.h"

int simpleScoreMeng(State* s)
{
	int stackIdx, freecellIdx;
	int score = 0;
	int columnCount = DECKSIZE;
	int colIdx, numIdx;
	
	for(stackIdx = 0; stackIdx < CELLS; stackIdx++)
	{
		if(stack[stackIdx] != -1)
		{
			score += 10 * deck[(int) s->stack[i]].num;
			columnCount -= deck[(int) s->stack[i]].num;
		}
	}
	
	for(freecellIdx = 0; freecellIdx < CELLS; freecellIdx++)
	{
		if(freecell[freecellIdx] != -1)
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
			if(deck[(int) column[colIdx][currentIdx]].num = deck[(int) column[colIdx][currentIdx+1]].num - 1)
			{
				score ++;
			}
			currentIdx ++;
			if(column[colIdx][currentIdx+1] == -1)
				break;
		}
	}
	return score;
}
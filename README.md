FREECELL SOLVER

This program goal is to solve FreeCell games.

The Algorithm
The program loads the initial state from a setup file. This initial state is stored in a struct called State. It has spots for the foundation cards, freecells, and columns. The foundation cards are called the stack. The state struct contains the number of the cards, 52 cards. The deck is the way each card is identified. States struct is a way to keep an array of State structs.

From a state there are functions for calculating the possible moves from that state, function to generate all those states, and a function to score the state. Functionality to create a hash table to lookup and store states are included in the stateHashTable.c file.

The algorithm uses those functions to search all possible moves of a state. This tree search is limited to a preset depth. This is because the possible moves blows up exponentially. This is accomplished through the function subtreeSearch. It is called recursively until no more moves or the MAX_DEPTH has been reached. The number of moves is further reduced by using the hash table. Using the hash table reduces the number of paths the computer has to follow because of repetitions. By using preset foundation spots for suits and sorting the freecell, the time required to search the tree is greatly reduced. The search will return the state with the highest score and the whole process is started again. Using another hash table we store the results of tree search and make sure we never repeat, circular condition.

The scoring function is the heart of the algorithm. The scoring allows the algorithm a strategy for solving the game. The most basic scoring would be to give an arbitrary number of points every time a card is moved to the foundation. This usually works for a couple of moves but will result in no solution in anything but the most simple games. This is due to depth that would be required to solve the game. The scoring function the algorithm uses does this basic scoring plus rewards stacking cards in the columns. This gives the scoring algorithm a non circular, a move that doesn't result in the same score, path to work at until more cards can be moved to the foundation. By choosing more complex scoring methods, games could be solved in less moves. The scoring function the algorithm uses also can reward making stacks on piles with no buried aces in them.

The algorithm can solve all 5 games with some modifications. Depth and the amount it takes away for stacking on an ace can be adjusted to solve each of the games.

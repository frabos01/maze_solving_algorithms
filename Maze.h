#include "ListArray.h"

/*
Labirinto a pianta rettangolare.
*/
typedef struct {
   int w, h;/* larghezza (numero di celle per riga) ed altezza (numero di celle per colonna) del labirinto */
   Cell** grid; /* griglia contenente la topologia del labirinto */
   Cell* start; /* puntatore alla cella d'inizio */
   Cell* end; /* puntatore alla cella da raggiungere */
} Maze;

char** saveMaze(FILE*, int*, int*);
bool isValid(int, int, int, int);
void createMazeGrid(char**, Maze*, int, int);
void findUnblockedNeighbours(const Cell*, List*);
void findUnvisitedNeighbours(const Cell*, bool**, List*);
bool withinOneMove(const Cell*, const Cell*);
bool areLinked(const Cell*, const Cell*);
bool isJunction(const Cell*, const Maze*);
bool isDeadEnd(const Cell*, const Maze*);
bool isBlocked(const Cell*);
void blockCell(Maze*, Cell*);
bool isWallRight(const Cell*, int);
bool isWallFront(const Cell*, int);
int turnClockwise(const Cell*, int);
int turnCounterClockwise(const Cell*, int);
Cell* moveForward(const Cell*, int);
void pruneSolution(List*);
void clean(char**, const Maze*);
void printSolution(char**, const Maze*, const List*);
void mazeFree(Maze*);
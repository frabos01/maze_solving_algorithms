#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "Maze.h"

/*Right Hand Wall Follower: algoritmo che risolve un labirinto che ha i muri connessi tra loro e le celle di partenza e di arrivo
  hanno un muro in almeno uno dei loro 4 lati. 
  Utilizza la seguente logica rispetto alla direzione di percorrenza ("regola della mano destra"):
  1) se non c'e' un muro a destra ruota di 90° in senso orario e si muove in avanti;
  2) se c'e' un muro a destra:
  		2.1) se non c'e' un muro di fronte si muove in avanti;
  		2.2) se c'e' un muro di fronte ruota di 90° in senso antiorario.

IP m Labirinto.
IOP solution Lista contenente la soluzione.
OP k Numero di passi.
*/
void RightHand(const Maze* m, List* solution, int* k){
	Cell* curr;
	int dir;
	srand(time(NULL)); /* inizializzo un random seed */
	initList(solution);
	dir = rand() % NEIGHBOURS;
	curr = m->start;
	listAdd(solution, curr);
	while(!isEqualCell(curr, m->end)){
		if(isWallRight(curr, dir)){
			if(isWallFront(curr, dir))
				dir = turnCounterClockwise(curr, dir);
			else{
				curr = moveForward(curr, dir);
				listAdd(solution, curr);
				(*k)++;
			}/* else */
		}/* if */
		else{
			dir = turnClockwise(curr, dir);
			curr = moveForward(curr, dir);
			listAdd(solution, curr);
			(*k)++;
		}/* else */
	}/* while */
	pruneSolution(solution);
}/* RightHand */

int main(int argc, char **argv){
    FILE* inF;
    List solution;
    int w, h, k;
    char** a;
    Maze m;
    k = 0;
    inF = fopen(argv[1], "r");
    if (inF == NULL)
        return -1;
    a = saveMaze(inF, &w, &h);
    createMazeGrid(a, &m, w, h);
    RightHand(&m, &solution, &k);
    printf("SOLUZIONE: ");
    printList(&solution);
    printSolution(a, &m, &solution);
    printf("\nPassi: %d\n", k);
    listFree(&solution);
    mazeFree(&m);
    return 0;
}/* main */
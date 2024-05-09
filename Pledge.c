#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "Maze.h"

/*Right Hand Wall Follower: algoritmo che risolve un labirinto che ha le mura connesse tra loro, opportunamente modificato per Pledge. 
  Utilizza la seguente logica rispetto alla direzione di percorrenza ("regola della mano destra"):
  1) se non c'e' un muro a destra ruota di 90° in senso orario e si muove in avanti;
  2) se c'e' un muro a destra:
  		2.1) se non c'e' un muro di fronte si muove in avanti;
  		2.2) se c'e' un muro di fronte ruota di 90° in senso antiorario.

IP m Labirinto.
IOP solution Lista contenente la soluzione.
IOP k Numero di passi.
IOP curr Cella corrente.
IOP count Contatore, viene incrementato di 1 per ogni rotazione in senso orario, decrementato di 1 per ogni rotazione in senso antiorario.
IP mainDir Direzione principale.
*/
void RightHand(const Maze* m, List* solution, int* k, Cell** curr, int* count, int mainDir){
	int dir;
	dir = turnCounterClockwise(*curr, mainDir);
	(*count)--;
	while(!isEqualCell(*curr, m->end) && *count != 0){
		if(isWallRight(*curr, dir)){
			if(isWallFront(*curr, dir)){
				dir = turnCounterClockwise(*curr, dir);
				(*count)--;
			}/* if */
			else{
				*curr = moveForward(*curr, dir);
				listAdd(solution, *curr);
				(*k)++;
			}/* else */
		}/* if */
		else{
			dir = turnClockwise(*curr, dir);
			(*count)++;
			*curr = moveForward(*curr, dir);
			listAdd(solution, *curr);
			(*k)++;
		}/* else */
	}/* while */
}/* RightHand */

/*Algoritmo di Pledge: progettato per aggirare gli ostacoli, richiede una direzione scelta arbitrariamente verso cui andare, 
  che sarà preferenziale. 
  Quando viene incontrato un ostacolo, una mano (la mano destra in questo implementazione) viene mantenuta lungo l'ostacolo,
  mentre vengono contati gli angoli ruotati (la rotazione in senso orario è positiva, la rotazione in senso antiorario è negativa).
  Quando il solutore si trova di nuovo nella direzione preferenziale originale, e la somma angolare delle virate eseguite è 0,
  il solutore lascia l'ostacolo e continua a muoversi nella sua direzione originale.
IP m Labirinto.
IOP solution Lista contenente la soluzione.
OP k Numero di passi. 
*/
void Pledge(const Maze* m, List* solution, int* k){
	Cell* curr;
	int dir, count, mainDir;
	srand(time(NULL)); /* inizializzo un random seed */
	initList(solution);
	mainDir = rand() % NEIGHBOURS;
	dir = mainDir;
	count = 0;
	curr = m->start;
	listAdd(solution, curr);
	while(!isEqualCell(curr, m->end)){
		if(!isWallFront(curr, mainDir)){
			curr = moveForward(curr, dir);
			listAdd(solution, curr);
			(*k)++;
		}/* if */
		else{
			RightHand(m, solution, k, &curr, &count, mainDir);
			dir = mainDir;
		}/* else */
	}/* while */
	pruneSolution(solution);
}/* Pledge */

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
    Pledge(&m, &solution, &k);
    printf("SOLUZIONE: ");
    printList(&solution);
    printSolution(a, &m, &solution);
    printf("\nPassi: %d\n", k);
    listFree(&solution);
    mazeFree(&m);
    return 0;
}/* main */
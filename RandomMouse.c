#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "Maze.h"

/* Random Mouse: il risolutore procede seguendo il passaggio corrente fino al raggiungimento di un incrocio,
   quindi prendere una decisione casuale sulla direzione successiva da seguire, evitando di girarsi di 180° (tornare indietro) ove possibile.
IP m Labirinto.
IOP solution Lista contenente la soluzione.
OP k Numero di passi.
*/
void RandomMouse(const Maze* m, List* solution, int* k){
	List ns;
	Cell* nxt;
    srand(time(NULL)); /* inizializzo un random seed */
    /* inizializzo la soluzione */
 	initList(solution);
    /* inizializzo la lista che contiene i vicini di una cella */
 	initList(&ns);
    /* aggiungo la cella iniziale alla soluzione */
 	listAdd(solution, m->start);
    /* itero finche' non raggiungo la fine */
 	while(!isEqualCell(solution->list[listIndex(solution, solution->length - 1)], m->end)){
        /* trovo i vicini della cella corrente */
        findUnblockedNeighbours(solution->list[listIndex(solution, solution->length - 1)], &ns);
        /* evita di tornare indietro */
        if(ns.length > 1 && solution->length > 1)
            listRemove(&ns, searchCell(solution->list[listIndex(solution, solution->length - 2)], &ns, ns.first));
        /* scelgo casualmente un vicino */
        nxt = chooseCell(&ns);
        /*libero la lista contenente le celle vicine alla corrente */
        listFree(&ns);
        /* aggiungo la prossima cella alla soluzione */
        listAdd(solution, nxt);
        /* incremento il numero di passi */
        (*k)++;
	}/* while */
    /* elimino le celle ripetute dalla soluzione */
    pruneSolution(solution);
}/* RandomMouse*/

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
    RandomMouse(&m, &solution, &k);
    printf("SOLUZIONE: ");
    printList(&solution);
    printSolution(a, &m, &solution);
    printf("\nPassi: %d\n", k);
    listFree(&solution);
    mazeFree(&m);
    return 0;
}/* main */
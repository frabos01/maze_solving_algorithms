#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "Maze.h"


/* Ritorna se trova vicoli ciechi.
IP m Labirinto.
IOP deadEnd Vicolo cieco.
OR Esito della ricerca. 
*/
bool findDeadEnd(const Maze* m, Cell** deadEnd){
	int i, j;
    for(i = 0; i < m->h; i++){
        for(j = 0; j < m->w; j++){
            if(isDeadEnd(&(m->grid[i][j]), m)){
            	*deadEnd = &(m->grid[i][j]);
                return true;
            }/* if */
        }/* for */
    }/* for */
    return false;
}/* findDeadEnd */

/* Riempie un passaggio.
IOP m Labirinto.
IP deadEnd Vicolo cieco.
*/
void fillPassage(Maze* m, Cell** deadEnd){
    List ns;
    /* dal vicolo cieco, mi muovo di una cella */
    findUnblockedNeighbours(*deadEnd, &ns);
    /* blocca il vicolo cieco */
    blockCell(m, *deadEnd);
    /* guardo alla prossima cella, se e' un vicolo cieco proseguo il ciclo */
    while(isDeadEnd(ns.list[0], m)){
        *deadEnd = ns.list[0];
        listFree(&ns);
        /* dal vicolo cieco, mi muovo di una cella */
        findUnblockedNeighbours(*deadEnd, &ns);
        /* blocca il vicolo cieco */
        blockCell(m, *deadEnd);
    }/* while */
}/* fillPassage */


/* Blocca tutti vicoli ciechi che si vengono a creare.
IOP m Labirinto.
OR Ritorna true se viene bloccata qualche cella, altrimenti false.
*/
void fillDeadEnds(Maze* m){
    Cell* deadEnd;
    deadEnd = NULL;
    while(findDeadEnd(m, &deadEnd)){
        fillPassage(m, &deadEnd);
    }
}/* fillDeadEnds */

/* DeadEndFiller: algoritmo di risoluzione per labirinti perfetti, riempie tutti i vicoli ciechi fino ad ottenere la soluzione.
IOP m Labirinto.
IOP solution Lista contenente la soluzione.
*/
void DeadEndFiller(Maze* m, List* solution){
    int i, j;
    fillDeadEnds(m);
    initList(solution);
    for(i = 0; i < m->h; i++){
        for(j = 0; j < m->w; j++){
            if(!isBlocked(&(m->grid[i][j])))
            	listAdd(solution, &(m->grid[i][j]));
        }/* for */
    }/* for */
}/* DeadEndFiller */

int main(int argc, char **argv){
    FILE* inF;
    List solution;
    int w, h;
    char** a;
    Maze m;
    inF = fopen(argv[1], "r");
    if (inF == NULL)
        return -1;
    a = saveMaze(inF, &w, &h);
    createMazeGrid(a, &m, w, h);
    DeadEndFiller(&m, &solution);
    printf("SOLUZIONE: ");
    printList(&solution);
    printSolution(a, &m, &solution);
    listFree(&solution);
    mazeFree(&m);
    return 0;
}/* main */

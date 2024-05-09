#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "Maze.h"

#define UNINITIALISED -1

/* Restituisce la cella vicina alla corrente a minore distanza dalla cella finale.
IP ce Cella corrente.
IP dist Array 2D contenente le distanze di ogni cella dalla cella finale.
OR Cella vicina alla corrente a minore distanza dalla cella finale.
*/
Cell* minDistNeigh(const Cell* ce, int** dist){
	List ns;
	Cell* min;
	int i;
	findUnblockedNeighbours(ce, &ns);
	min = ns.list[0];
	for(i = 1; i < ns.length; i++){
		if(dist[(ns.list[i])->row][(ns.list[i])->col] < dist[min->row][min->col])
			min = ns.list[i];
	}/* for */
	return min;
}/* minDistNeigh */

/* Flood Fill Algorithm: calcola la distanza di ogni cella dalla cella finale e poi costruisce la soluzione piu' breve passo a passo 
   a partire dalla cella di partenza.
IP m Labirinto.
IOP solution Lista contenente la soluzione.
*/
void floodFill(const Maze* m, List* solution){
	int** dist;
	int i, j;
	List ns, l;
	srand(time(NULL)); /* inizializzo un random seed */
	/* alloco ed inizializzo l'array contenente le distanze di ogni cella dalla cella finale */
	dist = malloc(m->h * sizeof(int*));
	for(i = 0; i < m->h; i++){
  		dist[i] = malloc(m->w * sizeof(int));
  		for(j = 0; j < m->w; j++)
  			dist[i][j] = UNINITIALISED;
  	}/* for */
  	/* inizializzo la lista */
  	initList(&l);
  	/* aggiungo la cella finale nella lista */
  	listAdd(&l, m->end);
  	/* imposto la sua distanza a 0 */
   	dist[(l.list[l.first])->row][(l.list[l.first])->col] = 0;
   	/* itero finche' la lista non e' vuota */
  	while(!isEmptyList(&l)){
  		/* trovo i vicini alla cella corrente */
  		findUnblockedNeighbours(l.list[l.first], &ns);
  		/* per ogni vicino calcolo la sua distanza dalla cella finale e lo inserisco nella lista */
  		for(i = 0; i < ns.length; i++){
  			if(dist[(ns.list[i])->row][(ns.list[i])->col] == UNINITIALISED){
  				listAdd(&l, ns.list[i]);
  				dist[(ns.list[i])->row][(ns.list[i])->col] = dist[(l.list[l.first])->row][(l.list[l.first])->col] + 1;
  			}/* if */
  		}/* for */
  	    listFree(&ns);
  	    /* rimuovo la cella corrente, la quale e' presente da piu' tempo nella lista */
  		listRemoveFirst(&l);
  	}/* while */
  	listFree(&l);
  	/* inizializzo la lista contenente la soluzione */
  	initList(solution);
  	/* ci aggiungo la cella di inizio */
  	listAdd(solution, m->start);
  	/* itero aggiungendo ogni volta  la cella a minore distanza dalla cella finale,
  	   parto dalla cella iniziale fino ad arrivare alla cella finale */
  	while(!isEqualCell(solution->list[solution->length - 1], m->end))
  		listAdd(solution, minDistNeigh(solution->list[solution->length - 1], dist));
}/* floodFill */


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
    floodFill(&m, &solution);
    printf("SOLUZIONE: ");
    printList(&solution);
    printSolution(a, &m, &solution);
    listFree(&solution);
    mazeFree(&m);
    return 0;
}/* main */
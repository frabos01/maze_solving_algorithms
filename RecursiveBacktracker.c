#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "Maze.h"

/* Recursive Backtracker: analogo ad una DFS in un grafo, che termina una volta raggiunta la cella finale.
IP m Labirinto.
IOP Lista contenente la soluzione.
OP Numero di passi.
*/
void RecursiveBacktracker(const Maze* m, List* solution, int* k){
  List ns;
  Cell* curr;
  int i, j;
  bool** visited;
  /* alloco ed inizializzo l'array che memorizza se una cella e' gia' stata visitata */
  visited = malloc(m->h * sizeof(bool*));
  for(i = 0; i < m->h; i++){
  	visited[i] = malloc(m->w * sizeof(bool));
  	for(j = 0; j < m->w; j++)
  		visited[i][j] = false;
  }/* for */
  srand(time(NULL)); /* inizializzo un random seed */
  /* inizializzo la lista che conterra' la soluzione, che si comporta come una pila */
  initList(solution);
  initList(&ns);
  /* aggiungo la cella iniziale */
  listAdd(solution, m->start);
  /* cella corrente, ultima inserita */
  curr = listGetLast(solution);
  /* contrassegno la cella come visitata */
  visited[curr->row][curr->col] = true;
  *k = 0;
  while(!isEqualCell(curr, m->end)){
    /* trovo i vicini alla cella corrente */
    findUnvisitedNeighbours(curr, visited, &ns);
    /* aggiungo un vicino alla cella corrente casuale alla pila, se non ci sono rimuovo l'ultimo elemento inserito nella pila*/
    if(!isEmptyList(&ns))
        listAdd(solution, chooseCell(&ns));
    else
        listRemoveLast(solution);
    listFree(&ns);
    /* cella corrente, ultima inserita */
    curr = listGetLast(solution);
    /* contrassegno la cella come visitata */
    visited[curr->row][curr->col] = true;
    /* incremento i passi */
    (*k)++;
  }/* while */
  for(i = 0; i < m->h; i++)
    free(visited[i]);
  free(visited);
}/* RecursiveBacktracker */

int main(int argc, char **argv){
    FILE* inF;
    Maze m;
    int w, h, k;
    List solution;
    char** a;
    inF = fopen(argv[1], "r");
    if (inF == NULL)
        return -1;
    a = saveMaze(inF, &w, &h);
    createMazeGrid(a, &m, w, h);
    k = 0;
    RecursiveBacktracker(&m, &solution, &k);
    printf("SOLUZIONE: ");
    printList(&solution);
    printSolution(a, &m, &solution);
    printf("\nPassi: %d\n", k);
    listFree(&solution);
    return 0;
}/* main */
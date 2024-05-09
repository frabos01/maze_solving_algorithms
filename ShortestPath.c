#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "Maze.h"

/* Shortest Path: equivalente ad una BFS, che si interrompe quando raggiunge la fine.
   Trova una delle soluzioni piu' brevi.
IP m Lista.
IOP solution Lista contenente la soluzione.
*/
void ShortestPath(const Maze* m, List* solution){
    int i, j;
    bool** visited;
    Cell*** prev;
    Cell* curr;
    List q, ns;
    srand(time(NULL)); /* inizializzo un random seed */
    /* alloco ed inizializzo l'array che tiene traccia se una cella e' gia' stata visitata */
    visited = malloc(m->h * sizeof(bool*));
    for(i = 0; i < m->h; i++){
        visited[i] = malloc(m->w * sizeof(bool));
        for(j = 0; j < m->w; j++)
            visited[i][j] = false;
   }/* for */
    /* alloco ed inizializzo l'array che memorizza la cella visitata precedentemente nella posizione corrispondente a quella appena visitata */
    prev = malloc(m->h * sizeof(Cell**));
    for(i = 0; i < m->h; i++)
        prev[i] = malloc(m->w * sizeof(Cell*));
    /* inizializzo una lista che si comporta come una coda */
    initList(&q);
    /* aggiungo la cella di inizio */
    listAdd(&q, m->start);
    /* itero finche' non raggiungo la fine */
    while(!isEqualCell(q.list[q.first], m->end)){
        /*cella corrente, cella da piu' tempo nella lista */
        curr = q.list[q.first];
        /* contrassegno la cella come visitata */
        visited[curr->row][curr->col] = true;
        /* trovo i vicini */
        findUnvisitedNeighbours(curr, visited, &ns);
        for(i = 0; i < ns.length; i++){
            /* salvo la cella corrente come precedente alle celle vicine */
            prev[ns.list[i]->row][ns.list[i]->col] = curr;
            /* aggiungo ogni vicino alla lista */
            listAdd(&q, ns.list[i]);
        }/* for */
        /* rimuovo la cella corrente dalla lista */
        listRemoveFirst(&q);
        listFree(&ns);
    }/* while */
    listFree(&q);
    /* inizializzo la soluzione */
    initList(solution);
    /* aggiungo la cella finale */
    listAddFirst(solution, m->end);
    curr = m->end;
    /* itero finche' non aggiungo la xella iniziale */
    while(!isEqualCell(curr, m->start)){
        /* aggiungo in prima posizione la cella che era stata visitata prima della cella corrente */
        listAddFirst(solution, prev[curr->row][curr->col]);
        /* la cella precedente diventa la corrente */
        curr = prev[curr->row][curr->col];
    }/* while */
    for(i = 0; i < m->h; i++)
        free(visited[i]);
    free(visited);
    for(i = 0; i < m->h; i++)
        free(prev[i]);
    free(prev);
}/* ShortestPath */

int main(int argc, char **argv){
    FILE* inF;
    Maze m;
    int w, h;
    List solution;
    char** a;
    inF = fopen(argv[1], "r");
    if (inF == NULL)
        return -1;
    a = saveMaze(inF, &w, &h);
    createMazeGrid(a, &m, w, h);
    ShortestPath(&m, &solution);
    printf("SOLUZIONE: ");
    printList(&solution);
    printSolution(a, &m, &solution);
    listFree(&solution);
    mazeFree(&m);
    return 0;
}/* main */
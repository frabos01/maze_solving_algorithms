#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "Maze.h"


/*Salva la linea guida da seguire per raggiungere la cella finale.
IP m Labirinto.
IOP path Lista contenente la linea guida.
*/
void drawGuidingLine(const Maze* m, List* path){
    Cell* curr;
    int rdiff, cdiff;
    curr = m->start;
    listAdd(path, curr);
    while(!isEqualCell(curr, m->end)){
        rdiff = 0;
        cdiff = 0;
        if (fabs(curr->row - (m->end)->row) > 0){
        	if(curr->row < (m->end)->row)
            	rdiff = 1;
            else 
            	rdiff = -1;
        }/* if */
        if (fabs(curr->col - (m->end)->col) > 0){
        	if(curr->col < (m->end)->col)
            	cdiff = 1;
            else 
            	cdiff = -1;
        }/* if */
        curr = &m->grid[curr->row + rdiff][curr->col + cdiff];
        listAdd(path, curr);
    }/* while */
}/* drawGuidingLine */

/* Tenta la mossa diretta se due celle consecutive della linea guida sono collegate tra loro.
IP m Labirinto.
IOP solution Lista contenente la soluzione.
IP curr Cella corrente.
IP Cella della linea guida successiva da raggiungere.
OR Spostamento avvenuto o no.
*/
bool tryDirectMove(const Maze* m, List* solution,  const Cell* curr, Cell* next){
    int rdiff, cdiff;
    rdiff = next->row - curr->row;
    cdiff = next->col - curr->col;
    /* calculate the cell next door and see if it's open */
    if (rdiff == 0 || cdiff == 0){
        if (areLinked(curr, next)){
            listAdd(solution, next);
            return true;
        }/* if */
    }/* if */
    else{
        if(areLinked(curr, &(m->grid[curr->row + rdiff][curr->col])) && areLinked(next, &(m->grid[curr->row + rdiff][curr->col]))){
            listAdd(solution, &m->grid[curr->row + rdiff][curr->col]);
            listAdd(solution, next);
            return true;
        }/* if */
        else if(areLinked(curr, &(m->grid[curr->row][curr->col + cdiff])) && areLinked(next, &(m->grid[curr->row][curr->col + cdiff]))){
            listAdd(solution, &m->grid[curr->row][curr->col + cdiff]);
            listAdd(solution, next);
            return true;
        }/* else if */
        else
            return false;
    }
	return false;
}/* tryDirectMove */

/* BacktrackingSolver: analogo ad una DFS in un grafo, che termina una volta raggiunta la cella finale.
IOP solution Lista contenente la soluzione.
IP goal Cella da raggiungere.
IP m Labirinto.
*/
void BacktrackingSolver(List* solution, const Cell* goal, const Maze* m){
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
  initList(&ns);
  /* cella corrente, ultima inserita */
  curr = listGetLast(solution);
  /* contrassegno la cella come visitata */
  visited[curr->row][curr->col] = true;
  while(!isEqualCell(curr, goal)){
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
  }/* while */
  for(i = 0; i < m->h; i++)
    free(visited[i]);
  free(visited);
}/* BacktrackingSolver */

/* Invio un robot per ogni vicino alla cella corrente, ognuno dei quali ha l'obiettivo di raggiungere la prossima cella della linea guida.
IOP solution Lista contenente la soluzione.
IP curr Cella corrente.
IP Cella della linea guida successiva da raggiungere.
IP m Labirinto.
*/
 void sendOutRobots(List* solution, const Cell* curr, Cell* next, const Maze* m){
    List ns, robotPath;
    List* robotPaths;
    int i;
    initList(&ns);
    findUnblockedNeighbours(curr, &ns);
    /* create a robot for each open neighbour */
    robotPaths = malloc(ns.length * sizeof(List));
    assert(robotPaths != NULL);
    for (i = 0; i < ns.length; i++){
        initList(&robotPath);
        listAdd(&robotPath, ns.list[i]);
        robotPaths[i] = robotPath;
    }/* for */

    /* randomly walk each robot, until it finds the guiding line or quits */
    for (i = 0; i < ns.length; i++)
        BacktrackingSolver(&robotPaths[i], next, m);
    /* add the shortest path to the solution */
    if(ns.length > 1)
        concat(solution, listMin(robotPaths, ns.length));
    else
        concat(solution, robotPaths);
    for (i = 0; i < ns.length; i++)
        listFree(&robotPaths[i]);
    listFree(&ns);
}/* sendOutRobots */ 

/* Chain Algorithm: traccia una linea guida (piu' dritta possibile) da seguire, che va dall'inizio alla fine. Quando si incontra un ostacolo
   invia dei robot che lo aggirano per arrivare alla prossima cella della linea guida. 
IP m Labirinto.
IOP solution Soluzione.
*/
void Chain(const Maze* m, List* solution){
    List guidingLine;
    int curr = 0;
    initList(&guidingLine);
    initList(solution);
    drawGuidingLine(m, &guidingLine);
    printf("GUIDING LINE: ");
    printList(&guidingLine);
    listAdd(solution, guidingLine.list[0]);
    while(curr < guidingLine.length - 1){
        if(!tryDirectMove(m, solution,  guidingLine.list[curr], guidingLine.list[curr + 1]))
            sendOutRobots(solution, guidingLine.list[curr], guidingLine.list[curr + 1], m);
        curr++;
    }/* while */
    listFree(&guidingLine);
    pruneSolution(solution);
}/* Chain */

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
    Chain(&m, &solution);
    printf("SOLUZIONE: ");
    printList(&solution);
    printSolution(a, &m, &solution);
    listFree(&solution);
    return 0;
}/* main */
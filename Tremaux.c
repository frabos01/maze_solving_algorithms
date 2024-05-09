#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "Maze.h"

/* Restituisce la prossima cella da visitare, utilizzando la logica di Tremaux.
IOP visitedCells Array 2D contenete nelle posizioni corrispondenti quante volte sono state visitate le celle.
IP ns Lista contente i vicini alla cella corrente.
IOP solution Lista contenente la soluzione.
OR Prossima cella da visitare.
*/
Cell* whatNext(int** visitedCells, const List* ns, List* solution){
        List* visitCounts;
        int i;
        Cell* ce;
        /* gestisco lo scenario in cui vi sia una sola cella vicina alla corrente*/
        if(ns->length == 1)
            ce = ns->list[0];
        else{
            /* organizzo i vicini in base a quante volte sono stati visitati */
            visitCounts = malloc(3 * sizeof(List));
            /* inizializzo un array di 3 liste: nell'indice 0 c'e' la lista che conterra' le celle mai visitate, all'indice 1 quella che 
            conterra' le celle visitate 1 volta, all'indice 2 quella che conterra' le celle visitate 2 volte */
            for(i = 0; i < 3; i++){
            initList(visitCounts + i);
            }/* for */
            for(i = 0; i < ns->length; i++)
                listAdd(visitCounts + visitedCells[(ns->list[i])->row][(ns->list[i])->col], ns->list[i]);

            /* 
            rispetto la regola di Tremaux quando mi trovo in una cella: se ci sono celle che non sono mai state visitate ne scelgo una,
            altrimenti scelgo una cella che e' gia' stata visitata solo una volta, altrimenti ancora scelgo una cella gia' visitata piu' volte.
            Rimuovo dalla lista dei vicini la cella precedentemente visitata. 
            */

            if(visitCounts->length != 0)
                ce = chooseCell(visitCounts);

            else if((visitCounts + 1)->length != 0){
                if((visitCounts + 1)->length > 1 && 
                    solution->length > 1 && 
                    isIn(visitCounts + 1, solution->list[solution->length - 2 ]))
                    listRemove(visitCounts + 1, searchCell(solution->list[solution->length - 2], visitCounts + 1, (visitCounts + 1)->first));
                ce = chooseCell(visitCounts + 1);
            }/* else if */
            
            else{
                if((visitCounts + 2)->length > 1 && 
                    solution->length > 1 && 
                    isIn(visitCounts + 2, solution->list[solution->length - 2 ]))
                    listRemove(visitCounts + 2, searchCell(solution->list[solution->length - 2], visitCounts + 2, (visitCounts + 2)->first));
                ce = chooseCell(visitCounts + 2);
            }/* else */

            for( i = 0; i < 3; i++)
                listFree(&visitCounts[i]);
            free(visitCounts);
        }/* else */
        return ce;
}/* whatNext */

/* Incremento le volte in cui e' stata visitata una cella di 1.
IP ce Cella visitata.
IOP visitedCells Array 2D contenete nelle posizioni corrispondenti quante volte sono state visitate le celle. 
*/
void visit(Cell* ce, int** visitedCells){   
    visitedCells[ce->row][ce->col] += 1;
}/* visit */

/* Algoritmo di Tremaux.
IP m Labirinto.
IOP solution Lista contenente la soluzione.
OP k Numero di passi eseguiti.
*/
void Tremaux(const Maze* m, List* solution, int* k){
    int i, j;
    List ns;
    Cell* next;
    int** visitedCells;
    srand(time(NULL)); /* Inizializzo un random seed */
    initList(solution);
    /* alloco ed inizializzo l'array 2D contenete nelle posizioni corrispondenti quante volte sono state visitate le celle.*/
    visitedCells = malloc(m->h * sizeof(int*));
    for(i = 0; i < m->h; i++){
        visitedCells[i] = malloc(m->w * sizeof(int));
        for(j = 0; j < m->w; j++)
            visitedCells[i][j] = 0;
    }/* for */

    /* prima mossa */
    listAdd(solution, m->start);
    visit(m->start, visitedCells);

    /* prendo un vicino a caso usando la logica di Tremaux, ripeto finche' arrivo alla fine */
    while(!isEqualCell(solution->list[solution->length - 1], m->end)){
        /* trovo i vicini alla cella corrente */
        findUnblockedNeighbours(solution->list[solution->length - 1], &ns);

        /* scelgo e visito la prossima cella basandomi sulla logica di Tremaux*/
        next = whatNext(visitedCells, &ns, solution);
        listFree(&ns);
        /* incremento i passi */
        (*k)++;
        /* aggiungo la cella alla soluzione*/
        listAdd(solution, next);
        /* aggiorno il numero di volte in cui e' stata  visitata la cella solo se la cella e' stata visitata meno di 2 volte */
        if(visitedCells[next->row][next->col] < 2)
            visit(next, visitedCells);
    }/* while */
    /* elimino dalla soluzione le celle presenti piu' volte */
    pruneSolution(solution);

    for(i = 0; i < m->h; i++)
        free(visitedCells[i]);
    free(visitedCells);
}/* Tremaux */

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
    Tremaux(&m, &solution, &k);
    printf("SOLUZIONE: ");
    printList(&solution);
    printSolution(a, &m, &solution);
    printf("\nPassi: %d\n", k);
    listFree(&solution);
    mazeFree(&m);
    return 0;
}/* main */
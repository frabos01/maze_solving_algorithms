#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "Maze.h"

/* Ritorna se due cammini distinti hanno un'intersezione.
IP l1 Lista contenente il primo cammino.
IP l2 Lista contenente il secondo cammino.
IP intersections Lista contenete le intersezioni tra due o piu' percorsi.
OR Esito della verifica.
*/
bool foundIntersection(const List* l1, const List* l2, const List* intersections){
    int j, i1, i2;
    for(j = 0; j < intersections->length; j++){
        i1 = searchCell(intersections->list[j], l1, 0);
        i2 = searchCell(intersections->list[j], l2, 0);
        if(i1 == i2 && i1 != -1)
            return true;
    }/* for */
    return false;
}/* foundIntersection */

/* Ritorna se due cammini sono uguali.
IP l1 Lista contenente il primo cammino.
IP l2 Lista contenente il secondo cammino.
OR Esito della verifica.
*/
bool isEqualPath(const List* p1, const List* p2){
    int i;
    if(p1->length == p2->length){
        for(i = 0; i < p1->length; i++){
            if(!isEqualCell(p1->list[listIndex(p1, i)], p2->list[listIndex(p2, i)]))
                return false;
        }/* for */
        return true;
    }/* if */
    return false;
}/* isEqualPath */

/* Ritorna se ci sono differenze tra due insiemi di percorsi.
IP paths1 Primo array di liste.
IP paths2 Secondo array di liste.
OR Esito del confronto.
*/
bool difference(const ListArray* paths1, const ListArray* paths2){
    int i, j;
    bool flag;
    if(paths1->size == paths2->size){
        for(i = 0; i < paths1->size; i++){
            flag = false;
            for(j = 0; j < paths2->size && !flag; j++){
                if(isEqualPath(&paths1->array[i], &paths2->array[j])){
                    flag = true;
                }
            }/* for */

            if(!flag)
                return true;

        }/* for */
        return false;
    }
    return true;
}/* difference */

/* Sistema le collisioni tra due percorsi bloccando la cella in cui si incontrano se questa NON e' una giunzione, la quale viene eliminata 
   dai percorsi.
IOP m Labirinto.
IOP paths Array di liste contenete i percorsi.
IOP intersections Puntatore alla lista contenete le intersezioni tra due o piu' percorsi.
IOP prev Array di liste che contiene le celle dei percorsi da cui si e' giunti alla cella di intersezione nella posizione 
   corrispondente.
*/
void fixCollisions(Maze* m, ListArray* paths, List* intersections, ListArray* prev){
    int i, j, index;
    Cell* ceI;
    Cell* prevI;
    Cell* ceJ;
    Cell*prevJ;
    List p;
    for(i = 0; i < paths->size - 1; i++){
        ceI = (paths->array + i)->list[listIndex(paths->array + i, (paths->array + i)->length - 1)];
        prevI = (paths->array + i)->list[listIndex(paths->array + i, (paths->array + i)->length - 2)];
        if(isDeadEnd(ceI, m))
            blockCell(m, ceI);
        else if(!isDeadEnd(ceI, m) && !isEqualCell(ceI, m->end)){
            for(j = i+1; j < paths->size; j++){
                ceJ = (paths->array + j)->list[listIndex(paths->array + j, (paths->array + j)->length - 1)];
                prevJ = (paths->array + j)->list[listIndex(paths->array + j, (paths->array + j)->length - 2)];
                if(!isDeadEnd(ceJ, m) && !isEqualCell(ceJ, m->end)){
                    if(isEqualCell(ceI, ceJ)){
                        if(isJunction(ceI, m)){
                            if(!isIn(intersections, ceI) && !isEqualCell(prevI, prevJ)){
                                listAdd(intersections, ceI);
                                initList(&p);
                                listAdd(&p, prevI);
                                listAdd(&p, prevJ);
                                listArrayAdd(prev, p);
                                listFree(&p);
                            }/* if */
                            if(isIn(intersections, ceI)){
                                index = searchCell(ceI, intersections, 0);
                                if(!isIn((prev->array + index), prevI))
                                    listAdd((prev->array + index), prevI);
                                if(!isIn((prev->array + index), prevJ))
                                    listAdd((prev->array + index), prevJ);
                            }/* if */
                        }/* if */
                        else{
                            if(!isEqualCell(prevI, prevJ)){
                                blockCell(m, ceI);
                                listRemoveLast(paths->array + i);
                                listRemoveLast(paths->array + j);
                            }/* if */
                        }/* else */
                        
                    }/* if */
                }/* if */
            }/* for */
        }/* else if */
    }/* for */
}/* fixCollisions */

/* Fa il primo passo, inondando le celle vicine alla cella di inizio, se possibile.
IOP m Labirinto.
IOP paths Array di liste contenete i percorsi.
IOP intersections Lista contenete le intersezioni tra due o piu' percorsi.
IOP prev Array di liste che contiene le celle dei percorsi da cui si e' giunti alla cella di intersezione nella posizione 
   corrispondente.
OR Ritorna true se almeno una nuova cella e' stata inondata, altrimenti false.
*/
bool oneTimeStepFirst(Maze* m, ListArray* paths, List* intersections, ListArray* prev){
    int j;
    Cell* ce;
    List ns;
    ListArray tempPaths;
    bool stepMade = false;
    initListArray(&tempPaths);
        /* ultima cella */
        ce = (paths->array)->list[listIndex(paths->array, (paths->array)->length - 1)];

            findUnblockedNeighbours(ce, &ns);
            if(ns.length > 1 && (paths->array)->length > 1)
                listRemove(&ns, searchCell((paths->array)->list[listIndex(paths->array, (paths->array)->length - 2)], &ns, ns.first));
    
            if(ns.length != 0){
                stepMade = true;
                for(j = 0; j < ns.length; j++){
                    listArrayAdd(&tempPaths, *(paths->array));
                    listAdd(tempPaths.array + tempPaths.size - 1, ns.list[j]);
                }/* for */
            }/* else */
    listFree(&ns);               
    if (!stepMade)
        return false;

    /* gestisce le collisioni */
    fixCollisions(m, &tempPaths, intersections, prev);

    listArrayFree(paths);
    initListArray(paths);
    copyListArray(paths, &tempPaths);
    listArrayFree(&tempPaths);
    return true;
}/* oneTimeStepFirst */

/* Se possibile inonda le celle non ancora visitate vicine all'ultima cella di ogni percorso.
IOP m Puntatore al labirinto.
IOP paths Array di liste contenete i percorsi.
IOP intersections Lista contenete le intersezioni tra due o piu' percorsi.
IOP prev Array di liste che contiene le celle dei percorsi da cui si e' giunti alla cella di intersezione nella posizione
   corrispondente.
OR Ritorna true se almeno una nuova cella e' stata inondata, altrimenti false.
*/
bool oneTimeStep(Maze* m, ListArray* paths, List* intersections, ListArray* prev){
    int i, j, index;
    Cell* ce;
    List ns;
    ListArray tempPaths;
    bool stepMade = false;
    initListArray(&tempPaths);
    
    for(i = 0; i < paths->size; i++){
        /* ultima cella */
        ce = (paths->array + i)->list[listIndex(paths->array + i, (paths->array + i)->length - 1)];
        
        if(isEqualCell(ce, m->end))
            listArrayAdd(&tempPaths, *(paths->array + i));
        else if((paths->array + i)->length != 0 && !isDeadEnd(ce, m) && !isBlocked(ce)){
            findUnblockedNeighbours(ce, &ns);
            if(ns.length > 1 && (paths->array[i]).length > 1){
                if(!isIn(intersections, ce))
                    listRemove(&ns, searchCell(paths->array[i].list[listIndex(&paths->array[i], paths->array[i].length - 2)], &ns, ns.first));
                else{
                    index = searchCell(ce, intersections, 0);
                    for(j = 0; j < (prev->array[index]).length; j++)
                        listRemove(&ns, searchCell((prev->array[index]).list[j], &ns, ns.first));
                }/* else */
            }/* if */
            if(ns.length != 0){
                stepMade = true;
                for(j = 0; j < ns.length; j++){
                    listArrayAdd(&tempPaths, paths->array[i]);
                    listAdd(tempPaths.array + tempPaths.size - 1, ns.list[j]);
                }/* for */
            }/* if*/
            listFree(&ns);
        }/*else if */
    }/* for */
    if (!stepMade)
        return false;

    /* gestisce le collisioni */
    fixCollisions(m, &tempPaths, intersections, prev);
    listArrayFree(paths);
    initListArray(paths);
    copyListArray(paths, &tempPaths);
    listArrayFree(&tempPaths);
    return true;
}/* oneTimeStep */

/* Inonda il labirinto step-by-step finche' e' possibile, cioe' finche' tutti i percorsi hannno raggiunto la cella finale oppure
   un vicolo cieco.
IOP m Labirinto.
IOP paths Array di liste contenete i percorsi.
*/
void floodMaze(Maze* m, ListArray* paths){
    List path, intersections;
    ListArray prev;
    bool stepMade;
    initListArray(&prev);
    initList(&path);
    initList(&intersections);
    listAdd(&path, m->start);
    /*printList(&path);*/
    listArrayAdd(paths, path);
    stepMade = oneTimeStepFirst(m, paths, &intersections, &prev);
    while(stepMade){
        stepMade = oneTimeStep(m, paths, &intersections, &prev);
    }/* while */
    listFree(&intersections);
    listArrayFree(&prev);
}/* floodMaze */

/* Sistema la lista di soluzioni eliminando i percorsi che terminano con un vicolo cieco, che ovviamente non sono soluzioni.
IOP m Labirinto.
IOP solutions Array di liste contenete le soluzioni.
*/
void fixSolutions(Maze* m, ListArray* solutions){
    int i;
    Cell* ce;
    i = 0;
    while(i < solutions->size){
        ce = (solutions->array + i)->list[((solutions->array + i)->length) - 1];
        if(!isEqualCell(ce, m->end))
            listArrayRemove(solutions, i);
        else
            i++;
    }/* while */
}/* fixSolutions */

/* Collision Solver: inonda il labirinto piu' volte finche' non vi sono piu' differenze di percorsi tra un'inondazione e la precedente.
   Vengono trovate piu' soluzioni tra cui tutte le soluzioni piu' brevi.
IOP m Labirinto.
IOP solutions Array di liste contenete le soluzioni.
*/
void CollisionSolver(Maze* m, ListArray* solutions){
    bool diff;
    ListArray tempPaths;
    initListArray(&tempPaths);
    /* Inonda il labirinto 2 volte e compara i risultati */
    floodMaze(m, solutions);
   
    floodMaze(m, &tempPaths);
     
    /* controllo se ci sono differenze tra le due liste di percorsi */
    diff = difference(solutions, &tempPaths);
    
    /* inonda il labirinto finche' non ci sono differenze */
    while(diff){
        
        listArrayFree(solutions);
        initListArray(solutions);
        copyListArray(solutions, &tempPaths);
        listArrayFree(&tempPaths);
        initListArray(&tempPaths);
        floodMaze(m, &tempPaths);
  
        diff = difference(solutions, &tempPaths); 
    }/* while */
    listArrayFree(&tempPaths);
    fixSolutions(m, solutions);
}/* CollisionSolver */

int main(int argc, char **argv){
    FILE* inF;
    ListArray solutions;
    int w, h, i;
    char** a;
    Maze m;
    srand(time(NULL)); /* inizializzo un random seed */
    inF = fopen(argv[1], "r");
    if (inF == NULL)
        return -1;
    a = saveMaze(inF, &w, &h);
    createMazeGrid(a, &m, w, h);
    initListArray(&solutions);
    CollisionSolver(&m, &solutions);
    printf("SOLUZIONI: ");
    printListArray(&solutions);
    for(i = 0; i < solutions.size; i++){
        printf("\n");
        printf("\n");
        printf("%d\n", i+1);
        printSolution(a, &m, &solutions.array[i]);
        printf("\n");
        clean(a, &m);
    }/* for */
    listArrayFree(&solutions);
    mazeFree(&m);
    return 0;
}/* main */
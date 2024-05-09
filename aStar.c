#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <limits.h>

#include "Maze.h"

/* Calcola la distanza di Manhattan tra due celle.
IP ce1 Prima cella.
IP ce2 Seconda cella.
OR Distanza di Manhattan.
*/
int ManhattanDist(const Cell* ce1, const Cell* ce2){
	return fabs(ce2->row - ce1->row) + fabs(ce2->col - ce1->col);
}/* ManhattanDist */

/* Inserisce nella posizione corretta un puntatore ad una cella in una lista, che si comporta come una coda di priorità
    implementata attraverso un min-heap, sfruttando la corrispondenza biunivoca tra heap ed array.
    - All'indice 0 si trova la radice dello heap.
    - I figli dell'elemento all'indice i si trovano agli indici 2i+1 e 2i+2.
    - Il padre dell'elemento all'indice i si trova all'indice (i-1)/2.
IOP l Lista in cui aggiungere l'elemento.
IP ce Elemento da aggiungere alla lista.
IP fScore Array 2D contenente il valore della funzione di costo di ogni cella.
*/
void listInsert(List* l, Cell* ce, int** fScore){
	int i;
	listAdd(l, ce);
	i = l->length - 1;
	while(i > 0 && (fScore[(l->list[(i-1)/2])->row][(l->list[(i-1)/2])->col] > fScore[(l->list[i])->row][(l->list[i])->col])){
		swap(l->list + i, l->list + (i-1)/2);
		i = (i - 1) / 2;
	}/* while */
}/* listInsert */

/* Rimuove l'elemento con priorita' massima(minima funzione di costo) dalla lista, che si comporta come una coda di priorità
   implementata attraverso un min-heap, sfruttando la corrispondenza biunivoca tra heap ed array.
   Mantiene invariata la proprietà dello heap.
   - All'indice 0 si trova la radice dello heap.
   - I figli dell'elemento all'indice i si trovano agli indici 2i+1 e 2i+2.
   - Il padre dell'elemento all'indice i si trova all' indice (i-1)/2.
IOP l Lista in cui aggiungere l'elemento.
IP fScore Array 2D contenente il valore della funzione di costo di ogni cella.
OR Cella con massima priorita'. 
*/
Cell* listRemoveMin(List* l, int** fScore){
	Cell* minCe;
	int i, j;
	minCe = l->list[0];
	l->list[0] = l->list[l->length - 1];
	l->length--;
	i = 0;
	if(2 * i + 1 <= l->length - 1){
		if(fScore[(l->list[2*i+1])->row][(l->list[2*i+1])->col] < fScore[(l->list[2*i+2])->row][(l->list[2*i+2])->col])
			j = 2*i+1;
		else
			j = 2*i+2;
	}/* if */
	while(2 * i + 1 <= l->length - 1 && fScore[(l->list[i])->row][(l->list[i])->col] > fScore[(l->list[j])->row][(l->list[j])->col]){
		swap(l->list + i, l->list + j);
		i = j;
		if(2 * i + 1 <= l->length - 1){
			if(fScore[(l->list[2*i+1])->row][(l->list[2*i+1])->col] < fScore[(l->list[2*i+2])->row][(l->list[2*i+2])->col])
				j = 2*i+1;
			else
				j = 2*i+2;
		}/* if */
	}/* while */
	return minCe;
}/* listRemoveMin */

/* Algoritmo A*.
   Sia c una cella del labirinto, introduco le seguenti funzioni:
   - g(c): distanza tra la cella c e la cella di partenza.
   - h(c): funzione euristica, distanza di Manhattan tra la cella c e la cella da raggiungere.
   - f(c) = g(c) + h(c): funzione di costo.
IP m Labirinto.
IOP solution Lista contenente la soluzione.
*/
void aStar(const Maze* m, List* solution){
	List p, ns;
	Cell* curr;
	Cell*** prev;
	int** gScore;
	int** fScore;
	int i, j, tempGscore, tempFscore;
	srand(time(NULL)); /* Inizializzo un random seed */
	initList(&p);/* Inizializzo la lista che si comportera' come coda di priorita' */
	initList(solution);/* Inizializzo la lista che conterra' la soluzione */

	/* alloco l'array 2D che conterra' in ogni posizione un puntatore alla cella precedentemente visitata, quindi il puntatore alla cella
	   dalla quale si e' arrivati a quella corrispondente */
	prev = malloc(m->h * sizeof(Cell**));
    for(i = 0; i < m->h; i++){
        prev[i] = malloc(m->w * sizeof(Cell*));
    }/* for */
    /* alloco ed inizializzo l'array 2D che conterra' in ogni posizione il valore di g(c) 
      (distanza tra la cella corrispondente e la cella di partenza) */
	gScore = malloc(m->h * sizeof(int*));
	for(i = 0; i < m->h; i++){
  		gScore[i] = malloc(m->w * sizeof(int));
  		for(j = 0; j < m->w; j++)
  			gScore[i][j] = INT_MAX;
	}/* for */
    /* alloco ed inizializzo l'array 2D che conterra' in ogni posizione la funzione di costo f(c) */
  	fScore = malloc(m->h * sizeof(int*));
	for(i = 0; i < m->h; i++){
  		fScore[i] = malloc(m->w * sizeof(int));
  		for(j = 0; j < m->w; j++)
  			fScore[i][j] = INT_MAX;
	}/* for */
  	/*g(start)*/
  	gScore[(m->start)->row][(m->start)->col] = 0;
  	/*f(start)*/
  	fScore[(m->start)->row][(m->start)->col] = ManhattanDist(m->end, m->start);
  	/* inserisco la cella di partenza nella coda di priorita' */
  	listInsert(&p, m->start, fScore);
  	/* inizializzo la cella corrente con la cella di partenza */
  	curr = m->start;
  	/* trovo i vicini della cella corrente */
  	findUnblockedNeighbours(curr, &ns);
  	/* per ogni vicino alla cella corrente calcolo g(c) e f(c) */
  	for(i = 0; i < ns.length; i++){
  		/* calcolo un valore temporaneo di g(c)*/
  		tempGscore = gScore[curr->row][curr->col] + 1;
  		/* calcolo un valore temporaneo di f(c) */
  		tempFscore = tempGscore + ManhattanDist(m->end, ns.list[i]);
  		/* aggiorno g(c) e f(c) se il valore calcolato e' minore di quello precedente */
  		if(tempFscore < fScore[(ns.list[i])->row][(ns.list[i])->col]){
  			gScore[(ns.list[i])->row][(ns.list[i])->col] = tempGscore;
  			fScore[(ns.list[i])->row][(ns.list[i])->col] = tempFscore;
  			/* inserisco la cella vicina nella coda di priorita' */
  			listInsert(&p, ns.list[i], fScore);
  			/* assegno la cella precedente nella posizione corrispondente alla cella vicina in questione*/
  			prev[(ns.list[i])->row][(ns.list[i])->col] = curr;
  		}/* if */
  	}/* for */
  	listFree(&ns);
  	/* itero finche' raggiungo la fine */
  	while(!isEqualCell(curr, m->end)){
  		/* rimuovo l'elemnto con priorita' massima */
  		curr = listRemoveMin(&p, fScore);
  		/* trovo i vicini della cella corrente */
  		findUnblockedNeighbours(curr, &ns);
  		/* per ogni vicino alla cella corrente calcolo g(c) e f(c) */
  		for(i = 0; i < ns.length; i++){
  			/* calcolo un valore temporaneo di g(c)*/
  			tempGscore = gScore[curr->row][curr->col] + 1;
  			/* calcolo un valore temporaneo di f(c)*/
  			tempFscore = tempGscore + ManhattanDist(m->end, ns.list[i]);
  			/* aggiorno g(c) e f(c) se il valore calcolato e' minore di quello precedente */
  			if(tempFscore < fScore[(ns.list[i])->row][(ns.list[i])->col]){
  				gScore[(ns.list[i])->row][(ns.list[i])->col] = tempGscore;
  				fScore[(ns.list[i])->row][(ns.list[i])->col] = tempFscore;
  				/* inserisco il puntatore alla cella vicina nella coda di priorita' */
  				listInsert(&p, ns.list[i], fScore);
  				/* assegno la cella precedente nella posizione corrispondente alla cella vicina in questione*/
  				prev[(ns.list[i])->row][(ns.list[i])->col] = curr;
  			}/* if */
  		}/* for */
  		listFree(&ns);
  	}/* while */
  	/* inserisco le celle appartenti alla soluzione in $solution */
  	while(!isEqualCell(curr, m->start)){
  		listAddFirst(solution, curr);
  		curr = prev[curr->row][curr->col];
  	}/* while */
}/* aStar */

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
    aStar(&m, &solution);
    printf("SOLUZIONE: ");
    printList(&solution);
    printSolution(a, &m, &solution);
    listFree(&solution);
    mazeFree(&m);
    return 0;
}/* main */
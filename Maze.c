#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <assert.h>


#include "Maze.h"


/* Salva un labirinto rappresentato tramite una griglia di caratteri a partire da un file di testo.
   Sono usati i seguenti caratteri del codice ASCII:
   x3 ' ': cella del labirinto,
   ' ': collegamento tra due celle adiacenti,
   '+': angoli delle celle,
   x3 '-': muro orizzontale,
   '|': muro verticale,
   'S': cella di partenza,
   'E': cella da raggiungere.
IF inF File di input.
OP w Larghezza.
OP h Altezza.
OR array 2D di caratteri contenente la topologia del labirinto.
*/
char** saveMaze(FILE* inF, int* w, int* h){
   char** a;
   int i, j;
   fscanf(inF, "%d", w);
   fscanf(inF, "%d", h);
   a = malloc(sizeof(char*) * (2 * (*h) + 1));
   for(i = 0; i < 2 * (*h) + 1; i++){
      a[i] = malloc((4 * (*w) + 2) * sizeof(char));
      for(j = 0; j < 4 * (*w) + 2; j++)
         fscanf(inF, "%c", &a[i][j]);
   }/* for */
   return a;
}/* saveMaze*/

/* Controllo se le coordinate della cella sono valide.
IP i Riga della cella.
IP j Colonna della cella.
IP w Larghezza del labirinto.
IP h Altezza del labirinto.
*/
bool isValid(int i, int j, int w, int h){
   return i >= 0 && j >= 0 && i < h && j < w;
}/* isValid */


/* Costruisco il labirinto a partire dall'array 2D di caratteri del codice ASCII contenente la sua topologia.
IP a Array2D contenente la rappresentazione grafica tramite caratteri del codice ASCII del labirinto.
OP maze Labirinto.
IP w Larghezza del labirinto.
IP h Altezza del labirinto.
*/
void createMazeGrid(char** a, Maze* maze, int w, int h){
   int i, j;
   maze->w = w;
   maze->h = h;
   maze->grid = malloc(sizeof(Cell*) * h);
   for(i = 0; i < h; i++)
      maze->grid[i] = malloc(sizeof(Cell) * w);

   for(i = 0; i < h; i++){
      for(j = 0; j < w; j++){

         /* Riga della cella */
         (maze->grid[i][j]).row = i;
         /* Colonna della cella */
         (maze->grid[i][j]).col = j;
         /* Cella di inizio */
         if(a[2*i+1][4*j+3] == 'S')
            maze->start = &(maze->grid[i][j]);
         /* Cella da raggiungere */
         else if(a[2*i+1][4*j+3] == 'E')
            maze->end = &(maze->grid[i][j]);
         /* Se la cella appartiene alla prima riga metto un muro in alto */
         if(i == 0){
            (maze->grid[i][j]).walls[UP] = 1;
            (maze->grid[i][j]).up = NULL;
         }/* if */
         /* Se la cella appartiene alla prima colonna metto un muro a sinistra */
         if(j == 0){
            (maze->grid[i][j]).walls[LEFT] = 1;
            (maze->grid[i][j]).left = NULL;
         }/* if */

         /* Ora basta inserire per ogni cella i muri in basso e a destra se presenti */

         /* Se ci sono tre '-' vicini nell'array $a allora vuol dire che è presente un muro orizzontale in basso nella cella corrente */
         if((a[2*i+2][4*j+3] == '-') && (a[2*i+2][4*j+2] == '-') && (a[2*i+2][4*j+4] == '-')){
            (maze->grid[i][j]).walls[DOWN] = 1;
            (maze->grid[i][j]).down = NULL;
            /* controllo che la cella in basso esista */
            if(isValid(i+1, j, w, h)){
               /* aggiungo un muro in alto alla cella che si trova in basso rispetto alla corrente */
               (maze->grid[i+1][j]).walls[UP] = 1;
               (maze->grid[i+1][j]).up = NULL;
            }/* if */
         }/* if */
         /* altrimenti la cella corrente comunica con quella in basso */
         else{
            (maze->grid[i][j]).walls[DOWN] = 0;
            (maze->grid[i][j]).down = &(maze->grid[i+1][j]);
            /* controllo che la cella in basso esista */
            if(isValid(i+1, j, w, h)){
               (maze->grid[i+1][j]).walls[UP] = 0;
               (maze->grid[i+1][j]).up = &(maze->grid[i][j]);
            }/* if */
         }/* else */
         /* Se c'e '|' nell'array $a allora vuol dire che è presente un muro vericale a destra nella cella corrente */
         if(a[2*i+1][4*j+5] == '|'){
            (maze->grid[i][j]).walls[RIGHT] = 1;
            (maze->grid[i][j]).right = NULL;
            /* controllo che la cella in basso esista */
            if(isValid(i, j+1, w, h)){
               /* aggiungo un muro a sinistra alla cella che si trova a destra rispetto alla corrente */
               (maze->grid[i][j+1]).walls[LEFT] = 1;
               (maze->grid[i][j+1]).left = NULL;
            }/* if */
         }/* if */
         /* altrimenti la cella corrente comunica con quella a destra */
         else{
            (maze->grid[i][j]).walls[RIGHT] = 0;
            (maze->grid[i][j]).right = &(maze->grid[i][j+1]);
            /* controllo che la cella in basso esista */
            if(isValid(i, j+1, w, h)){
               (maze->grid[i][j+1]).walls[LEFT] = 0;
               (maze->grid[i][j+1]).left = &(maze->grid[i][j]);
            }/* if */
         }/* else */
      }/*for*/
   }/*for*/
}/* createMazeGrid */


/* Trova tutte le celle vicine alla cella corrente; visitate, o no.
IP ce Cella.
IOP ns Lista contente i puntatori alle celle vicine alla cella puntata da $ce.
*/
void findUnblockedNeighbours(const Cell* ce, List* ns) {
   initList(ns);
   /* controllo in alto */
   if (!ce->walls[UP]){
      listAdd(ns, ce->up);
   }
   /* controllo in basso */
   if (!ce->walls[DOWN]){
      listAdd(ns, ce->down);
   }
   /* controllo a sinistra*/
   if(!ce->walls[LEFT]){
      listAdd(ns, ce->left);
   }
   /*controllo a destra*/
   if (!ce->walls[RIGHT]){
      listAdd(ns, ce->right);
   }
   if(ns->length != 0)
      shuffle(ns);
}/* findUnblockedNeighbors */

/* Trova i vicini di una cella che non sono ancora stati visitati.
IP ce Cella.
IP visited Array 2D che tiene traccia se una cella e' stata visitata o no nella posizione corrispondente alle sue coordinate.
IOP ns Lista contenente i vicini non visitati.
*/
void findUnvisitedNeighbours(const Cell* ce, bool** visited, List* ns) {
   initList(ns);
   /* controllo in alto */
   if (!ce->walls[UP] && !visited[ce->row-1][ce->col]){
      listAdd(ns, ce->up);
   }
   /* controllo in basso */
   if (!ce->walls[DOWN] && !visited[ce->row+1][ce->col]){
      listAdd(ns, ce->down);
   }
   /* controllo a sinistra*/
   if(!ce->walls[LEFT] && !visited[ce->row][ce->col-1]){
      listAdd(ns, ce->left);
   }
   /*controllo a destra*/
   if (!ce->walls[RIGHT] && !visited[ce->row][ce->col+1]){
      listAdd(ns, ce->right);
   }
   if( ns->length == NEIGHBOURS - 1){
      ns->list = realloc(ns->list, sizeof(Cell) * ns->length);
   }
   if(!isEmptyList(ns))
        shuffle(ns);
}/* findUnvisitedNeighbors */

/* Verifico se due celle distano una mossa.
IP ce1 Prima cella.
IP ce2 Seconda cella.
OR Ritorna true se due celle sono distano una mossa, altrimenti false.
*/
bool withinOneMove(const Cell* ce1, const Cell* ce2){
   return ( ((ce2->row - ce1->row == 0) && (fabs(ce2->col - ce1->col) == 1)) 
          || ((ce2->col - ce1->col == 0) && (fabs(ce2->row - ce1->row) == 1)) );

}/* withinOneMove */


/* Verifica se due celle sono collegate direttamente.
IP ce1 Prima cella.
IP ce2 Seconda cella.
OR Ritorna true se due celle sono collegate, altrimenti false.
*/
bool areLinked(const Cell* ce1, const Cell* ce2){
   if(isEqualCell(ce1, ce2))
      return true;
   if(withinOneMove(ce1, ce2)){
      if((ce2->row - ce1->row) == 1)
         return !ce1->walls[DOWN];
      else if((ce2->row - ce1->row) == -1)
         return !ce1->walls[UP];
      else if((ce2->col - ce1->col) == 1)
         return !ce1->walls[RIGHT];
      else
         return !ce1->walls[LEFT];
   }/* if */
   return false;
}/* areLinked */


/* Ritorna se una cella è una giunzione: è collegata con 3 o 4 celle.
IP ce Cella in questione.
OR Ritorna true se la cella è bloccata, false altrimenti.
*/
bool isJunction(const Cell* ce, const Maze* m){
   int sum, i;
   sum = 0;
   for(i = 0; i < NEIGHBOURS; i++)
      sum += ce->walls[i];
   return ((sum <= 1) && !isEqualCell(ce, m->end) && !isEqualCell(ce, m->start));
}/* isJunction */

/* Ritorna se una cella è un vicolo cieco: ha un muro in 3 dei suoi lati.
IP ce Cella in questione.
OR Ritorna true se la cella è bloccata, false altrimenti.
*/
bool isDeadEnd(const Cell* ce, const Maze* m){
   int sum, i;
   sum = 0;
   for(i = 0; i < NEIGHBOURS; i++)
      sum += ce->walls[i];
   /* una cella e' un vicolo cieco se ha 3 muri */
   return ((sum == NEIGHBOURS - 1) && !isEqualCell(ce, m->end) && !isEqualCell(ce, m->start));

}/* isDeadEnd */

/* Ritorna se una cella è bloccata su tutti i suoi lati.
IP ce Cella in questione.
OR Ritorna true se la cella è bloccata, false altrimenti.
*/
bool isBlocked(const Cell* ce){
   int sum, i;
   sum = 0;
   for(i = 0; i < NEIGHBOURS; i++)
      sum += ce->walls[i];
   /* una cella e' un vicolo cieco se ha 4 muri */
   return sum == NEIGHBOURS;
}/* isBlocked */

/* Blocca una cella.
IOP m Labirinto.
IOP Cella da bloccare.
*/
void blockCell(Maze* m, Cell* ce){
    
    if(ce->walls[UP] == 0){
        ce->walls[UP] = 1;
        if(isValid(ce->row - 1, ce->col, m->w, m->h))
            (ce->up)->walls[DOWN] = 1;
    }/* if */
    
    if(ce->walls[DOWN] == 0){
        ce->walls[DOWN] = 1;
        if(isValid(ce->row + 1, ce->col, m->w, m->h))
            (ce->down)->walls[UP] = 1;
    }/* if */

    if(ce->walls[LEFT] == 0){   
        ce->walls[LEFT] = 1;
        if(isValid(ce->row, ce->col - 1, m->w, m->h))
            (ce->left)->walls[RIGHT] = 1;
    }/* if */

    if(ce->walls[RIGHT] == 0){
        ce->walls[RIGHT] = 1;
        if(isValid(ce->row, ce->col + 1, m->w, m->h))
            (ce->right)->walls[LEFT] = 1;
    }/* if */
}/* blockCell */


/* Controlla se c'e' un muro a destra.
IP ce Cella.
IP dir Direzione attuale.
OR Esito della verifica.
*/
bool isWallRight(const Cell* ce, int dir){
   return ce->walls[(dir + 1) % NEIGHBOURS];
}/* is WallRight */

/* Controlla se c'e' un muro di fronte.
IP ce Cella.
IP dir Direzione attuale.
OR Esito della verifica
*/
bool isWallFront(const Cell* ce, int dir){
   return ce->walls[dir];
}/* is WallFront */

/* Gira in senso orario.
IP ce Cella.
IP dir Direzione attuale.
OR Nuova direzione.
*/
int turnClockwise(const Cell* ce, int dir){
   return (dir + 1) % NEIGHBOURS;
}/* turnClockwise */

/* Gira in senso antiorario.
IP ce Cella.
IP dir Direzione attuale.
OR Nuova direzione.
*/
int turnCounterClockwise(const Cell* ce, int dir){
   return (dir + NEIGHBOURS - 1) % NEIGHBOURS;
}/* turnCounterClockwise */

/* Muove avanti di una cella.
IP ce Cella.
IP dir Direzione attuale.
OR Nuova cella.
*/
Cell* moveForward(const Cell* ce, int dir){
   if(dir == UP)
      return ce->up;
   else if (dir == RIGHT)
      return ce->right;
   else if(dir == DOWN)
      return ce->down;
   else
      return ce->left;
}/* moveForward */

/* Elimina le celle duplicate dalla soluzione.
IOP solution Lista contenente le celle appartenenti alla soluzione.
*/
void pruneSolution(List* solution){
   int i, first_i, last_i, index;
   bool found = true;
   int attempt = 0;
   int maxAttempt = solution->length;

   while(found && solution->length > 1 && attempt < maxAttempt) {
      found = false;
      attempt++;

      for(i = 0; i < solution->length - 1; i++) {
         index = searchCell(solution->list[listIndex(solution, i)], solution, listIndex(solution, i+1));
         if (index != -1) {
            first_i = listIndex(solution, i);
            last_i = listIndex(solution, index);
            found = true;
            break;
         }/* if */
      }/* for */
      if(found)
         reduceList(solution, first_i, last_i);
   }/* while */

   solution->list = realloc(solution->list,  solution-> length * sizeof(Cell));
}/* pruneSolution */
   
/* Ripulisce la rappresentazione grafica del labirinto da celle contrassegnate con 'X'.
IOP a Array2D contenente la rappresentazione grafica tramite caratteri del codice ASCII del labirinto.
IP m Labirinto.
*/
void clean(char** a, const Maze* m){
   int i, j;
   for(i = 0; i < m->h; i++){
      for(j = 0; j < m->w; j++){
         if(a[2*i+1][4*j+3] == 'X')
            a[2*i+1][4*j+3] = ' ';
      }/* for */
   }/* for */
}/* clean */

/* Stampa a video il labirinto con la soluzione, contrassegnando con 'X' le celle appartenenti ad essa.
IP a Array2D contenente la rappresentazione grafica tramite caratteri del codice ASCII del labirinto.
IP m Labirinto.
IP solution Lista contenente le celle appartenenti alla soluzione.
OV Labirinto con la soluzione.
*/
void printSolution(char** a, const Maze* m, const List* solution ){
   int i, j;
   for(i = 0; i < m->h; i++){
      for(j = 0; j < m->w; j++){
         if(isIn(solution, &m->grid[i][j]) &&  a[2*i+1][4*j+3] != 'S' &&  a[2*i+1][4*j+3] != 'E')
            a[2*i+1][4*j+3] = 'X';
      }/* for */
   }/* for */
   for(i = 0; i < 2 * m->h + 1; i++){
      for(j = 0; j < 4 * m->w + 2; j++){
         printf("%c", a[i][j]);
      }/* for */
   }/* for */
}/* printSolution */

/* Libera la memoria occupata dal labirinto.
IOP m Labirinto da liberare.
*/
void mazeFree(Maze* m){
   int i;
   for(i = 0; i < m->h; i++)
      free(m->grid[i]);
   free(m->grid);
   m->h = 0;
   m->w = 0;
   m->start = NULL;
   m->end = NULL;
}/* mazeFree */
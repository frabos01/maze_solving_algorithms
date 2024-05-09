#include <stdbool.h>
#include <stdlib.h>

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
#define NEIGHBOURS 4

/* 
Cella.
row Riga.
col Colonna.
up Puntatore alla cella in alto, se collegata direttamente.
down Puntatore alla cella in basso, se collegata direttamente.
left Puntatore alla cella a sinistra, se collegata direttamente.
right Puntatore alla cella a destra, se collegata direttamente.
walls Array che indica la presenza di un muro rispettivamente in alto, a destra, in basso e a sinistra.
      1: muro presente.
      0: muro assente.
*/
typedef struct Cell{
   int row;
   int col;
   struct Cell* up;
   struct Cell* down;
   struct Cell* left;
   struct Cell* right;
   int walls[NEIGHBOURS];
}Cell;

void swap(Cell**, Cell**);
bool isEqualCell(const Cell*, const Cell*);
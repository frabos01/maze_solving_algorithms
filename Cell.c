#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "Cell.h"


/*Scambio tra loro due celle.
IOP a Prima cella.
IOP b Seconda cella.
*/
void swap(Cell** a, Cell** b){
   Cell* temp;
   temp = *a;
   *a = *b;
   *b = temp;
}/* swap */


/*Restituisce se due celle sono uguali tra loro.
IP p1 Prima cella.
IP p2 Seconda cella.
OR Esito del confronto.
*/
bool isEqualCell(const Cell* p1, const Cell* p2){
   return (p1->row == p2->row) && (p1->col == p2->col);
}/* isEqualCell */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "List.h"


/* Inizializza una lista.
IOP l Lista da inizializzare.
*/
void initList(List* l){
    l->list = malloc(sizeof(Cell*));
    assert(l->list != NULL);
    l->capacity = 1;
    l->length = 0;
    l->first = 0;
}/* initList */

/* Restituisce se una lista e' vuota.
IOP l Lista da esaminare.
OR Esito della verifica.
*/
bool isEmptyList(const List* l){
    return l->length == 0;
}/* isEmptyList */

/* Svuota la lista e libera la sua memoria allocata dinamicamente.
IOP l Lista da liberare.
*/
void listFree(List* l) {
    free(l->list);
    l->length = 0;
} /* listFree */

/* Ritorna l'indice che dista $i posizioni dall'inizio della lista.
IP l Lista da cui individuare l'indice dell'elemento $i-esimo.
IP i Indice dell'elemento da individuare.
OR L'indice contenente che dista $i posizioni dall'inizio della lista $l.
*/
int listIndex(const List* l, int i) {
    return (l->first + i) % l->capacity;
} /* listIndex */

/* Raddoppia la capienza della lista quando questa e` piena.
IOP l Lista di cui raddoppiare la capienza.
*/
void doubleIfFull(List* l) {
    int i;
    Cell** newList;
    if (l->length == l->capacity) {
        newList = malloc(2 * l->capacity * sizeof(Cell*));
        assert(newList != NULL);
        for(i = 0; i < l->length; i++)
            newList[i] = l->list[listIndex(l, i)];
        l->capacity *= 2;
        l->first = 0;
        free(l->list);
        l->list = newList;
    } /* if */
} /* doubleIfFull */


/* Aggiunge un elemento in fondo alla lista.
IOP l Lista a cui aggiungere un elemento.
IP elem Elemento da aggiungere alla lista.
*/
void listAdd(List* l, Cell* elem) {
    doubleIfFull(l);
    l->list[listIndex(l, l->length)] = elem;
    l->length++;
} /* listAdd */

/* Aggiunge un elemento all'inizio della lista.
IOP l Lista a cui aggiungere un elemento.
IP elem Elemento da aggiungere alla lista.
*/
void listAddFirst(List* l, Cell* elem) {
    int i;
    doubleIfFull(l);
    for(i = listIndex(l, l->length - 1); i >= l->first; i--)
        l->list[i+1] = l->list[i];
    l->list[l->first] = elem;
    l->length++;
} /* listAddFirst */

/* Rimuove l'i-esimo elemento della lista.
IOP l Lista da cui rimuovere un elemento.
IP index 
*/
void listRemove(List* l, int index) {
    int i;
    if(index >= 0){
        for(i = listIndex(l, index); i < listIndex(l, l->length - 1); i++)
            l->list[listIndex(l, i)] = l->list[listIndex(l, i+1)];
        l->length--;
    }/* if */
    /*realloc(l->list, l->length * sizeof(Cell));*/
} /* listRemove */

/* Rimuove il primo elemento della lista.
IOP l Lista da cui rimuovere l'elemento.
*/
void listRemoveFirst(List* l) {
    if(!isEmptyList(l)){
        l->length --;
        l->first = listIndex(l, 1);
    }/* if */
} /* listRemoveFirst */

/* Rimuove l'ultimo elemento della lista.
IOP l Lista a cui rimuovere l'elemento.
*/
void listRemoveLast(List* l) {
    l->length --;
} /* listRemoveFirst */

/* Mescola casualmente gli elementi di una lista.
IOP Lista da mescolare.
*/
void shuffle(List* l) {
   int i, j;
   for(i = l->length - 1; i > 0; i--) {
      j = rand() % (i + 1);
      swap(l->list + i, l->list + j);
   }/* for */
}/* shuffle */

/* Sceglie un elemento a caso di una lista.
IP Lista dalla quale scegliere una cella.
OR Cella scelta.
*/
Cell* chooseCell(const List* l) {
    int randomIndex;
    /* Genera un indice casuale nell'intervallo [0, l->length - 1] */
    randomIndex = rand() % l->length;

    /* Restituisce l'elemento corrispondente all'indice casuale */
    return l->list[randomIndex];
}/* chooseCell */

/* Effettua una ricerca lineare su una lista.
IP key cella da cercare.
IP l Lista nella quale cercare $key.
IP start Indice da cui iniziare la ricerca.
OR Indice in cui è presente la cella, altrimenti -1.
*/
int searchCell(const Cell* key, const List* l, int start) {
    int i;
    if(l->length > 0){
        for(i = listIndex(l, l->length - 1); i >= listIndex(l, start); i--) {
            if(isEqualCell(key, l->list[i]))
                return i;
        } /* for */
    }/* if */
   return -1;
} /* searchCell */

/* Riduce una lista eliminando gli elementi da first + 1 a last.
IOP l Lista da ridurre.
IP first Indice dell'ultimo elemento da tenere.
IP last Indice dell'ultimo elemento da eliminare.
*/
void reduceList(List* l, int first, int last) {
   int i;
   for(i = 1; last + i <= l->length - 1; i++)
      l->list[listIndex(l, first + i)] = l->list[listIndex(l, last + i)];
   l->length = l->length - last + first;
}/* reduceList */

/* Restituisce se una cella è presente in una lista.
IP l Lista in cui cercare.
IP ce Cella da cercare.
OR Esito della ricerca.
*/
bool isIn(const List* l, const Cell* ce){
    return searchCell(ce, l, l->first) != -1;
}/* isIn */

/*Ritorna l'ultimo elemento aggiunto.
IP l Lista da cui individuare l'ultimo elemento.
OR Ultimo elemento.
*/
Cell* listGetLast(const List* l){
    return l->list[listIndex(l, l->length - 1)];
}/* listGetLast */

/*Ritorna il primo elemento aggiunto.
IP l Lista da cui individuare il primo elemento.
OR Primo elemento.
*/
Cell* listGetFirst(const List* l){
    return l->list[l->first];
}/* listGetLast */

/* Stampa a video una lista.
IP l Lista da stampare.
OV Contenuto della lista.
*/
void printList(const List* l){
    int i;
    printf("[");
    if(l->length > 0){
        printf("(%d, %d)", (l->list[l->first])->row, (l->list[l->first])->col);
        for(i = 1; i < l->length; i++)
        printf(", (%d, %d)", (l->list[listIndex(l, i)])->row, (l->list[listIndex(l, i)])->col);
    }/* if */
     printf("]\n");
}/* printList */

/* Concatena la seconda lista alla prima.
IOP l1 Prima lista.
IP l2 Seconda lista.
*/
void concat(List* l1, List* l2){
    int i;
    for(i = 0; i < l2->length; i++)
        listAdd(l1, l2->list[listIndex(l2, i)]);
}/* concat */

/* Copia la seconda lista nella prima.
IOP l1 Prima lista.
IP l2 Seconda lista.
*/
void copyList(List* l1, const List* l2){
    int i;
    initList(l1);
    for(i = 0; i < l2->length; i++){
        listAdd(l1, l2->list[i]);
    }
}/* copyList */
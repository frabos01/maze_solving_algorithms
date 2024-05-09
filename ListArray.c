#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "ListArray.h"

/* Inizializza un array di liste.
IOP Array di liste.
*/
void initListArray(ListArray* l){
    l->array = malloc(sizeof(List));
    assert(l->array != NULL);
    l->capacity = 1;
    l->size = 0;
}/* initListArray */

/* Svuota l'array e libera la sua memoria allocata dinamicamente.
IOP l Array di liste da liberare.
*/
void listArrayFree(ListArray* l) {
	int i;
	for(i = 0; i < l->size; i++)
    	listFree(&(l->array[i]));
    free(l->array);
} /* listOfListFree */

/* Verifico se un array di liste è vuoto.
IP l Array di liste.
OR esito della verifica.
*/
bool isEmptyListArray(const ListArray* l){
    return l->size == 0;
}/* isEmptyListArray */

/* Raddoppia la capienza dell'array di liste lista quando questo e` pieno.
IOP l Array di liste di cui raddoppiare la capienza.
*/
void doubleListArray(ListArray* l) {
    int i;
    List* newList;
    if (l->size == l->capacity) {
        newList = malloc(2 * l->capacity * sizeof(List));
        assert(newList != NULL);
        for(i = 0; i < l->size; i++){
            /*initList(newList + i);*/
            copyList(newList + i, l->array + i);
        }
        l->capacity *= 2;
        listArrayFree(l);
        l->array = newList;
    } /* if */
}/* doubleListArray */

/* Aggiunge un elemento all'array di liste.
IOP l Array di liste a cui aggiungere un elemento.
IP elem Elemento da aggiungere.
*/
void listArrayAdd(ListArray* l, const List elem) {
    doubleListArray(l);
    copyList(l->array + l->size, &elem);
    l->size++;
} /* listAdd */

/* 
Elimina un elemento dall'array di liste.
IOP l Array di liste da cui eliminare un elemento.
IP index Indice dell'elemento da eliminare.
*/
void listArrayRemove(ListArray* l, int index){
    int i;
    for(i = index; i < l->size - 1; i++){
        listFree(l->array + i);
        copyList(l->array + i, l->array + i + 1);
    }
    l->size--;
}/* listArrayRemove */

/* Scambia due liste.
IOP l1 Primo elemento da scambiare.
IOP l2 Secondo elemento da scambiare.
*/
void swapList(List* l1, List* l2) {
    List temp;
    temp = *l1;
    *l1 = *l2;
    *l2 = temp;
} /* swapList */

/* Usa l'ultimo elemento come pivot, e poi:
~ Sposta gli elementi piu` piccoli a sinistra del pivot.
~ Sposta gli elementi piu` grandi a destra.
~ Sposta il pivot al confine tra i due insiemi suddetti.
IP l Array di liste da partizionare.
IP n Lunghezza dell'array di liste.
OR Indice del pivot.
*/
int partition(List l[], int n) {
    int i, indicePivot;
    List *pAI = l; /* Puntatore ad l[i], con i = 0, .., n - 1 */
    List *pASwap = l; /* Puntatore all'elemento di $l che sara` scambiato con a[i] */
    List *pPivot = l + n - 1; /* Puntatore al pivot, che e` l[n - 1] */
    indicePivot = 0;
    for (i = 0; i < n; i++) {
        if (pAI->length < pPivot->length) {
            swapList(pAI, pASwap);
            pASwap ++;
            indicePivot++;
        } /* if */
        pAI++;
    } /* for */
    swapList(pPivot, pASwap);
    return indicePivot;
} /* partition */

/* Restituisce il puntatore alla k-esima lista piu' corta.
IP l Array di indirizzi da elaborare.
IP n Grandezza di a.
IP k Indice dell'indirizzo da ritornare. 0 <= k < n
OR Lista con la k-esima lunghezza piu' corta.
*/
List* quickSelect(List l[], int n, int k) {
    int indicePivot = partition(l, n);
    if (indicePivot == k) 
        return l + indicePivot;
    else if (indicePivot < k) /* Cerca l'elemento in l[indicePivot + 1, ..., n - 1] */
        return quickSelect(l + indicePivot + 1, n - indicePivot - 1, k - indicePivot - 1);
    else /* Cerca l'elemento in l[0, ..., indicePivot - 1] */
        return quickSelect(l, indicePivot, k);
} /* quickSelect */

/* Restituisce un puntatore alla lista di lunghezza minore.
IP l Array su cui cercare.
IP n Lunghezza dell'array.
OR Lista di lunghezza minore.
*/
List* listMin(List l[], int n){
    return quickSelect(l, n, 0);
}/* listMin */

/* Copia il secondo array di liste nel primo.
IOP l1 Primo array di liste.
IP l2 Secondo array di liste.
*/
void copyListArray(ListArray* l1, const ListArray* l2){
    int i;
    l1->size = 0;
    for(i = 0; i < l2->size; i++)
        listArrayAdd(l1, l2->array[i]);
}/* copyListArray */

/* Stampa su video un array di liste.
IP l Array di liste da stampare.
OV Contenuto dell'array di liste. 
*/
void printListArray(const ListArray* l){
    int i;
    printf("[ ");
    printList(l->array);
    for(i = 1; i < l->size; i++){
        printf(", ");
        printList(l->array + i);
    }/* for */
    printf(" ]\n");
}/* printListArray */
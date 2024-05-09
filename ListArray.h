#include "List.h"

/*
   Array di liste.
*/
typedef struct {
   List* array;/* array contenente le liste */
   int size;/* grandezza dell'array di liste */
   int capacity;/* capacità dell'array di liste */
} ListArray;

void initListArray(ListArray*);
void listArrayFree(ListArray*);
bool isEmptyListArray(const ListArray*);
void doubleListArray(ListArray*);
void listArrayAdd(ListArray*, const List);
void listArrayRemove(ListArray*, int);
void swapList(List*, List*);
int partition(List[], int);
List* quickSelect(List[], int, int);
List* listMin(List[], int);
void copyListArray(ListArray*, const ListArray*);
void printListArray(const ListArray*);
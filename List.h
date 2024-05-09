#include "Cell.h"

/*
   Lista di celle.
*/
typedef struct {
   Cell** list;/* lista contenente le celle */
   int length;/* lunghezza (grandezza) della lista */
   int capacity;/* capacità della lista */
   int first; /* Indice dell'elemento che e` nella lista da piu` tempo. */
} List;


void initList(List*);
bool isEmptyList(const List*);
void listFree(List*);
int listIndex(const List*, int);
void doubleIfFull(List*);
void listAdd(List*, Cell*);
void listAddFirst(List*, Cell*);
void listRemove(List*, int);
void listRemoveFirst(List*);
void listRemoveLast(List*);
void shuffle( List*);
Cell* chooseCell(const List*);
int searchCell(const Cell*, const List*, int);
void reduceList(List*, int, int);
bool isIn(const List*, const Cell*);
Cell* listGetLast(const List*);
Cell* listGetFirst(const List*);
void printList(const List*);
void concat(List*, List*);
void copyList(List*, const List*);
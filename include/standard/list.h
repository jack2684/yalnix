#ifndef _LIST_H
#define _LIST_H
#include "node.h"
typedef struct y_List {
    int         size;
    int         rc;
    node_t      *head;    
    node_t      *tail;    
} list_t;

list_t *list_init();
int list_clear(list_t *list);
int list_destroy(void *list);
int list_pushFront(list_t *list, node_t *node);
int list_pushRear(list_t *list, node_t *node);
node_t* list_findIdx(list_t *list, int idx);
int list_insert(list_t *list, node_t *node, int idx);
node_t* list_popFront(list_t *list);
node_t* list_rmIdx(list_t *list, int idx);
node_t* list_popRear(list_t *list);
#endif


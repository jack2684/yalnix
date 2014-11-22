/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#ifndef _LIST_H
#define _LIST_H
#include "common.h"

#define ERR_NULL_POINTER 1
#define ERR_OUT_OF_RANGE 2

typedef struct y_Node {
    void            *data;
    struct y_Node   *next;
} node_t;

node_t *node_init(void *data);




typedef struct y_List {
    int         size;
    int         rc;
    node_t      *head;    
    node_t      *tail;    
} list_t;

list_t *list_init();
int list_clear(list_t *list);
int list_destroy(void *list);
node_t *list_add_head(list_t *list, void *data);
node_t *list_add_tail(list_t *list, void *data);
node_t *list_find_idx(list_t *list, int idx);
node_t *list_insert(list_t *list, void *data, int idx);
void* list_rm_head(list_t *list);
void* list_rm_idx(list_t *list, int idx);
void* list_rm_tail(list_t *list);

#endif


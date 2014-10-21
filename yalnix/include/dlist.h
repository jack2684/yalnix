#ifndef _DLIST_H
#define _DLIST_H
#include "common.h"

#define ERR_NULL_POINTER 1
#define ERR_OUT_OF_RANGE 2

typedef struct y_DNode {
    void             *data;
    struct y_DNode   *next;
    struct y_DNode   *prev;
} dnode_t;

dnode_t *dnode_init(void *data);

typedef struct y_DList {
    int         size;
    int         rc;
    dnode_t     *head;    
    dnode_t     *tail;    
} dlist_t;

dlist_t     *dlist_init();
int         dlist_clear(dlist_t *list);
int dlist_destroy(void *list);
dnode_t *dlist_add_head(dlist_t *list, dnode_t *node);
dnode_t *dlist_add_tail(dlist_t *list, dnode_t *node);
dnode_t *dlist_insert(dlist_t *list, dnode_t *node);
void*    dlist_rm_head(dlist_t *list);
void*    dlist_rm_tail(dlist_t *list);
void*    dlist_rm_this(dlist_t *list, dnode_t *node);

#endif


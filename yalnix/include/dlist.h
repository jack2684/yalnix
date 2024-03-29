/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#ifndef _DLIST_H
#define _DLIST_H
#include "common.h"
#include "hardware.h"

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
int         dlist_destroy(dlist_t *list);
dnode_t     *dlist_add_head(dlist_t *list, void *data);
dnode_t     *dlist_add_tail(dlist_t *list, void *data);
dnode_t     *dlist_insert(dlist_t *list, void *data, int idx);
void        *dlist_rm_head(dlist_t *list);
void        *dlist_rm_head(dlist_t *list);
void        *dlist_rm_tail(dlist_t *list);
void        *dlist_rm_idx(dlist_t *list, int idx);
void        *dlist_rm_this(dlist_t *list, dnode_t *node);
int         dlist_is_empty(dlist_t *list);

#endif


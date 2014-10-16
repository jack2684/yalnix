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
int list_add_head(list_t *list, node_t *node);
int list_add_tail(list_t *list, node_t *node);
node_t* list_find_idx(list_t *list, int idx);
int list_insert(list_t *list, node_t *node, int idx);
node_t* list_rm_head(list_t *list);
node_t* list_rm_idx(list_t *list, int idx);
node_t* list_rm_tail(list_t *list);

typedef struct y_DList {
    int         size;
    int         rc;
    dnode_t     *head;    
    dnode_t     *tail;    
} dlist_t;

dlist_t     *dlist_init();
int         dlist_clear(list_t *list);
int         dlist_add_head(list_t *list, node_t *node);
int         dlist_add_tail(list_t *list, node_t *node);
dnode_t*    dlist_rm_head(list_t *list);
dnode_t*    dlist_rm_tail(list_t *list);

#endif


/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#ifndef _HEAP_H
#define _HEAP_H
#include "common.h"

#define ERR_NULL_POINTER 1
#define ERR_OUT_OF_RANGE 2

#define INIT_SIZE 8

enum heap_type {
    MAX,
    MIN,
}

typedef struct y_heap {
    int             size;
    int             rc;
    enum heap_type  type;       // Max heap? Min heap?
    void            **datas; 
} heap_t;

heap_t *heap_init();
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


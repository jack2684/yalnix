#include "aux.h"

dlist_t *id_generator_init(int size) {
    if(size <= 0) {
        log_err("Size is negative!!");
        return NULL;
    } 

    dlist_t *id_list = dlist_init();
    if(id_list == NULL) {
        log_err("Cannot create id_list for some reason");
        return NULL;
    } 
    
    int i;
    int* d;
    for(i = 0; i < size; i++) {
        id_generator_push(id_list, i);
    }

    return id_list;
}

int id_generator_pop(dlist_t *id_list) {
    if(id_list->size <= 0) {
        log_err("Nothing to pop from id_list when size is %d!", id_list->size);
        return 1;
    }

    void *d = dlist_rm_head(id_list);
    return *((int*)d);
}

int id_generator_push(dlist_t *id_list, int id) {
    int *d = (int*) malloc(sizeof(int));
    *d = id;
    dlist_add_tail(id_list, (void*)d); 
}


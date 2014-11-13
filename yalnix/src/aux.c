#include "aux.h"

hashmap_t *idp = NULL;
dlist_t *id_list = NULL;

dlist_t *id_generator_init(int size) {
    if(size <= 0) {
        log_err("Size is not positive!!");
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
        return -1;
    }

    void *d = dlist_rm_head(id_list);
    return *((int*)d);
}

int id_generator_push(dlist_t *id_list, int id) {
    int *d = (int*) malloc(sizeof(int));
    *d = id;
    dlist_add_tail(id_list, (void*)d); 
}

int aux_get_next_id(dlist_t *id_list) {
    if(id_list == NULL) {
        id_list = id_generator_init(MAX_RESROUCE);
    }
    if(idp == NULL) {
        idp = hashmap_init();
        if(idp == NULL) {
            log_err("Cannot init hash map id pool");
            return -1;
        }   
    }
    return id_generator_pop(id_list);
}

int util_add(int id, void *data, enum util_type type) {
    int rc = 0;
    util_t *util = (util_t*)malloc(sizeof(util_t));
    if(util == NULL) {
        log_err("Cannot malloc util");
        return 1;
    }
    util->type = type;
    util->data = data;
    rc = hashmap_put(idp, id, util);
    if(rc) {
        log_err("Hash map put err, key %d", id);
        return 1;
    }
    return 0;
}

int util_put(int id, void *data) {
    int rc = 0;
    util_t *util = (util_t*)malloc(sizeof(util_t));
    if(util == NULL) {
        log_err("Cannot malloc util");
        return 1;
    }
    util->type = NA;
    util->data = data;
    rc = hashmap_put(idp, id, util);
    if(rc) {
        log_err("Hash map put err, key %d", id);
        return 1;
    }
    return 0;
}

void *util_get(int id) {
    util_t *util = hashmap_get(idp, id);
    log_info("UTIL Hashmap get done %p", util);
    if(util == NULL) {
        log_err("Cannot get data from hashmap, key %d", id);
        return NULL;
    }
    log_info("Going to return util data %p", util->data);
    return util->data;
}

void *util_rm(int id) {
    util_t *util = hashmap_rm(idp, id);
    if(util == NULL) {
        log_err("Cannot remove from hashmap, key %d", id);
        return NULL;
    }
    void *data = util->data;
    free(util);
    return data;
}

int util_new_id() {
    return aux_get_next_id(id_list);
}

int util_reclaim_id(int id) {
    id_generator_push(id_list, id);
}


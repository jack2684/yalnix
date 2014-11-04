/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "list.h"
#include "common.h"
//#include "stdlib.h"     // For the use of malloc, will be replace with yalnix malloc 

list_t *list_init() {
    list_t *list = (list_t*)malloc(sizeof(list_t));
    if(list) {
        list->size = 0; 
        list->rc = 0; 
        list->head = NULL;
        list->tail = NULL;
        return list;
    } else {
        list->rc = ERR_NULL_POINTER; 
        return NULL;
    }
}

int list_clear(list_t *list) {
    if(!list) {
        list->rc = ERR_NULL_POINTER; 
        return ERR_NULL_POINTER;
    }
    node_t *p = list->head, *prev;
    while(p != NULL) {
        prev = p;
        p = p->next;
        free(prev);
        list->size--;
    }
    list->head = NULL;
    list->tail = NULL;
    return 0;
}

int list_destroy(void *list) {
    int rc = list_clear((list_t*)list);
    if(rc) {
        return rc;
    } else {
        free((list_t*)(list));
        list = NULL;
        return 0;
    }
}

node_t *list_add_head(list_t *list, void *data) {
    return list_insert(list, data, 0);
}

node_t *list_add_tail(list_t *list, void *data) {
    return list_insert(list, data, list->size);
}

node_t* list_find_idx(list_t *list, int idx) {
    log_info("Inside %s with list size %d", __func__, list->size);
    if(!list) {
        list->rc = ERR_NULL_POINTER;
        log_err("list pointer is NULL");
        return NULL;
    }
    if(idx >= list->size || idx < 0) {
        list->rc = ERR_OUT_OF_RANGE; 
        log_err("idx %d out of list range[0, %d)", idx, list->size);
        return NULL;
    }

    log_info("About to find idx %d", idx);
    int i = 0;
    node_t *node = list->head;
    while(i < idx) {
        log_info("iter %d, the node pointer is %p", i, node);
        node = node->next;
        i++;
    }
    log_info("idx %d found", idx);
    return node;
}

node_t *list_insert(list_t *list, void* data, int idx) {
    log_info("Inside %s", __func__);
    node_t* node = node_init(data);
    log_info("NOde init done");
    if(!list->size) {       // Handle empty list
        list->head = node;
        list->tail = node;
    } else if (idx == 0) {  // Handle head insert
        node->next = list->head;
        list->head = node;
    } else {                // Normal case
        log_info("Finding prev node");
        node_t *prev = list_find_idx(list, idx - 1);
        log_info("prev node Found");
        if(!prev) {
            log_err("Prev node is NULL for some reason");
            return NULL;
        }
        node->next = prev->next;
        prev->next = node;
    }

    list->size++;
    return node;
}

/* I use wrote this instead of using list_rm_idx
 * because I don't want malloc inside a malloc call.
 * 
 * The node_t* prev = node_init(NULL) in list_rm_idx
 * might triger a malloc call, and ends in infinite 
 * malloc recursion
 */
void* list_rm_head(list_t *list) {
    if(!list) {
        list->rc = ERR_NULL_POINTER;
        return NULL;
    }
    if(list->size) {
        void* data = list->head->data;
        node_t *node = list->head;
        list->head = list->head->next;
        free(node);
        return data;
    } else {
        return NULL;
    } 
    //return list_rm_idx(list, 0);
}

/* Noted that, list_rmIdx do not free the memory of the removed node data!
 */
void* list_rm_idx(list_t *list, int idx) {
    log_info("Inside %s", __func__);
    if(!list) {
        list->rc = ERR_NULL_POINTER;
        log_err("List pointer is NULL");
        return NULL;
    }
    if(idx >= list->size || idx < 0) {
        list->rc = ERR_OUT_OF_RANGE; 
        log_err("List input idx %d out of range", idx);
        return NULL;
    }

    // Locate index, noted that we should not use malloc here
    // because this function can be called by malloc, which results in 
    // infinite recursion
    node_t* prev = node_init(NULL);
    prev->next = list->head;
    int i = 0;
    while(i < idx) {
        prev = prev->next;
        i++;
    }

    // Sanity check
    if(prev->next == list->head) {
        list->head = list->head->next;
    }
    if(prev->next == list->tail) {
        list->tail = prev;
    }
   
    // Removing node
    node_t *rmNode = prev->next;
    prev->next = rmNode->next;
    list->size--;
    
    void *data = rmNode->data;
    free(rmNode);
    log_info("List remove data %d", *((int*)data));
    return data;
}

void* list_rm_tail(list_t *list) {
    return list_rm_idx(list, list->size - 1);
}

void list_print(list_t *list) {
    if(!list) {
        printf("NULL list\n");
        return;
    }
 
    node_t *p = list->head;
    printf("size: %d\n", list->size);
    printf("[");
    while(p) {
        printf("%d,", (*((int*)p->data)));
        p = p->next;
    }
    printf("]\n");
}

int list_is_empty(list_t *list) {
    if(list->head && list->tail && list->size) {
        return 0;
    } else if (!list->head && !list->tail && !list->size) {
        return 1;
    } else {
        _debug("List inconsistency occurs!\n");
        return -1;
    }
}

node_t *node_init(void* data) {
    node_t *node = (node_t*)malloc(sizeof(node_t));
    if(node) {
        node->data = data;
        node->next = NULL;
        return node;
    } else {
        return NULL;
    }   
}





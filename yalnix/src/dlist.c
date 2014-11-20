/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "dlist.h"
//#include "stdlib.h"     // For the use of malloc, will be replace with yalnix malloc 

/* Init doubly list
 */
dlist_t *dlist_init() {
    dlist_t *list = (dlist_t*)malloc(sizeof(dlist_t));
    if(list) {
        list->size = 0; 
        list->rc = 0; 
        list->head = NULL;
        list->tail = NULL;
        return list;
    } else {
        log_err("Cannot malloc for dlist!!!!");
        return NULL;
    }
}

int dlist_clear(dlist_t *list) {
    if(!list) {
        return ERR_NULL_POINTER;
    }
    dnode_t *p = list->head, *prev;
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

int dlist_destroy(dlist_t *list) {
    int rc = dlist_clear(list);
    if(rc) {
        return rc;
    } else {
        free(list);
        list = NULL;
        return 0;
    }
}

dnode_t *dlist_add_head(dlist_t *list, void *data) {
    return dlist_insert(list, data, 0);
}

dnode_t *dlist_add_tail(dlist_t *list, void *data) {
    return dlist_insert(list, data, list->size);
}

dnode_t* dlist_find_idx(dlist_t *list, int idx) {
    if(!list) {
        list->rc = ERR_NULL_POINTER;
        return NULL;
    }
    if(idx >= list->size || idx < 0) {
        list->rc = ERR_OUT_OF_RANGE; 
        return NULL;
    }

    int i = 0;
    dnode_t *node = list->head;
    while(i < idx) {
        node = node->next;
        i++;
    }
    return node;
}

dnode_t *dlist_insert(dlist_t *list, void* data, int idx) {
    dnode_t* node = dnode_init(data);
    if(!list->size) {               // Handle empty list
        list->head = node;
        list->tail = node;
    } else if (idx == 0) {          // Handle head insert
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
    } else if (idx == list->size) { // Handle tail insert
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
    } else {                        // Normal case
        dnode_t *prev = dlist_find_idx(list, idx - 1);
        dnode_t *next = prev->next;
        if(!prev || !next) {
            _debug("Insert error!\n");
            return NULL;
        }
        prev->next = node;
        node->prev = prev;
        node->next = next;
        next->prev = node;
    }

    list->size++;
    return node;
}

void* dlist_rm_head(dlist_t *list) {
    //if(!list) {
    //    list->rc = ERR_NULL_POINTER;
    //    return NULL;
    //}
    //if(list->size) {
    //    void* data = list->head->data;
    //    dnode_t *node = list->head;
    //    list->head = list->head->next;
    //    return data;
    //} else {
    //    return NULL;
    //} 
    return dlist_rm_idx(list, 0);
}

/* Noted that, dlist_rmIdx do not free the memory of the removed node!
 */
void* dlist_rm_idx(dlist_t *list, int idx) {
    if(!list) {
        list->rc = ERR_NULL_POINTER;
        return NULL;
    }
    if(idx >= list->size || idx < 0) {
        list->rc = ERR_OUT_OF_RANGE; 
        return NULL;
    }
    dnode_t *rm_node;
    void    *data;

    // Special cases
    if(list->size == 1) {                // Only node
        rm_node = list->head;
        data = rm_node->data;
        list->head = NULL;
        list->tail = NULL;
        free(rm_node);
        list->size = 0;
        return data;
    } else if (idx == 0) {               // Remove the head
        rm_node = list->head;
        data = rm_node->data;
        list->head = rm_node->next;
        list->head->prev = NULL;
        free(rm_node);
        list->size--;
        return data;
    } else if (idx == list->size - 1) { // Remove the tail
        rm_node = list->tail;
        data = rm_node->data;
        list->tail = rm_node->prev;
        list->tail->next = NULL;
        free(rm_node);
        list->size--;
        return data;
    }

    // Locate index
    dnode_t* prev = dnode_init(NULL);
    prev->next = list->head;
    int i = 0;
    while(i < idx) {
        prev = prev->next;
        i++;
    }

    // Removing node
    rm_node = prev->next;
    prev->next = rm_node->next;
    data = rm_node->data;
    free(rm_node);
    list->size--;
    return data;
}

void *dlist_rm_this(dlist_t *list, dnode_t *node) {
    if(!list) {
        list->rc = ERR_NULL_POINTER;
        return NULL;
    }
    void *data = node->data;
    if(node->prev) {
        node->prev->next = node->next;
    } else if (list->head == node) {
        list->head = node->next;
    } else {
        log_err("Node is not the head of this list");
        return NULL;
    }
    if(node->next) {
        node->next->prev = node->prev;
    } else if (list->tail == node) {
        list->tail = node->prev;
    } else {
        log_err("Node is not the tail of this list");
        return NULL;
    }
    free(node);
    list->size--;
    return data;
}

void* dlist_rm_tail(dlist_t *list) {
    return dlist_rm_idx(list, list->size - 1);
}

void dlist_print(dlist_t *list) {
    if(!list) {
        printf("NULL list\n");
        return;
    }
 
    dnode_t *p = list->head;
    printf("size: %d\n", list->size);
    printf("[");
    while(p) {
        printf("%d,", (*((int*)p->data)));
        p = p->next;
    }
    printf("]\n");
}

int dlist_is_empty(dlist_t *list) {
    if(list->head && list->tail && list->size) {
        return 0;
    } else if (!list->head && !list->tail && !list->size) {
        return 1;
    } else {
        _debug("List inconsistency occurs!\n");
        return -1;
    }
}

dnode_t *dnode_init(void* data) {
    dnode_t *node = (dnode_t*)malloc(sizeof(dnode_t));
    if(node) {
        node->data = data;
        node->next = NULL;
        node->prev = NULL;
        return node;
    } else {
        return NULL;
    }   
}

void *dlist_peek_head(dlist_t *list) {
    dnode_t *n = list->head;
    return n->data;
}


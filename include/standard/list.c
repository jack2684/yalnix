#include "../standardLib.h"
#include "stdlib.h"     // For the use of malloc, will be replace with yalnix malloc 

list_t *list_init() {
    list_t *list = (list_t*)malloc(sizeof(list_t));
    if(list) {
        list->size = 0; 
        list->rc = 0; 
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
    }
    list->size = 0;
    return 0;
}

int list_deleteall(list_t *list) {
    list->rc = list_clear(list);
    if(list->rc) {
        free(list);
        list = NULL;
        return 0;
    } else {
        return list->rc;
    }
}

int list_pushFront(list_t *list, node_t *node) {
    if(!list) {
        list->rc = ERR_NULL_POINTER; 
        return ERR_NULL_POINTER;
    }
    node->next = list->head;
    list->head = node;
    list->size++;
    return 0;
}


int list_pushRear(list_t *list, node_t *node) {
    if(!list) {
        list->rc = ERR_NULL_POINTER; 
        return ERR_NULL_POINTER;
    }
    if(list->tail) {
        list->tail->next = node;
    } 
    node->next = NULL;
    list->tail = node;
    list->size++;
    return 0;
}

node_t* list_findIdx(list_t *list, int idx) {
    if(!list) {
        list->rc = ERR_NULL_POINTER;
        return NULL;
    }
    if(idx >= list->size || idx < 0) {
        list->rc = ERR_OUT_OF_RANGE; 
        return NULL;
    }

    int i = -1;
    node_t *node = NULL;
    while(i < idx) {
        if(!node) {
            node = list->head;
        } else {
            node = node->next;
        }
        i++;
    }
    return node;
}

int list_insert(list_t *list, node_t *node, int idx) {
    if(idx == 0) {
        return list_pushFront(list, node);        
    }
    if(idx == list->size) {
        return list_pushRear(list, node);
    }

    node_t *prev = list_findIdx(list, idx - 1);
    if(!node) {
        return list->rc;
    }
    node->next = prev->next;
    prev->next = node;
    return 0;
}

node_t* list_popFront(list_t *list) {
    if(!list || !list->size) {
        list->rc = ERR_NULL_POINTER;
        return NULL;
    }
    node_t *node = list->head;
    list->head = list->head->next;
    list->size--;
    if(list->size == 0) {
        list->tail = NULL;
    }
    node->next = NULL;
    return node;
}

/* Noted that, list_rmIdx do not free the memory of the removed node!
 */
node_t* list_rmIdx(list_t *list, int idx) {
    if(!list) {
        list->rc = ERR_NULL_POINTER;
        return NULL;
    }
    if(idx >= list->size || idx < 0) {
        list->rc = ERR_OUT_OF_RANGE; 
        return NULL;
    }

    if(idx = 0) {
        return list_popFront(list);
    } else {
        node_t* prev = NULL;
        int i = -1;
        while(i < idx - 1) {
            if(!prev) {
                prev = list->head;
            } else {
                prev = prev->next;
            }
            i++;
        }
        node_t *rmNode = prev->next;
        prev->next = rmNode->next;
        rmNode->next = NULL;
        list->size--;
        return rmNode;
    }
}

node_t* list_popRear(list_t *list) {
    return list_rmIdx(list, list->size - 1);
}




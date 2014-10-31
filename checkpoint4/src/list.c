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
    if(!list) {
        list->rc = ERR_NULL_POINTER;
        return NULL;
    }
    if(idx >= list->size || idx < 0) {
        list->rc = ERR_OUT_OF_RANGE; 
        return NULL;
    }

    int i = 0;
    node_t *node = list->head;
    while(i < idx) {
        node = node->next;
        i++;
    }
    return node;
}

node_t *list_insert(list_t *list, void* data, int idx) {
    node_t* node = node_init(data);
    if(!list->size) {       // Handle empty list
        list->head = node;
        list->tail = node;
    } else if (idx == 0) {  // Handle head insert
        node->next = list->head;
        list->head = node;
    } else {                // Normal case
        node_t *prev = list_find_idx(list, idx - 1);
        if(!prev) {
            return NULL;
        }
        node->next = prev->next;
        prev->next = node;
    }

    list->size++;
    return node;
}

void* list_rm_head(list_t *list) {
    //if(!list) {
    //    list->rc = ERR_NULL_POINTER;
    //    return NULL;
    //}
    //if(list->size) {
    //    void* data = list->head->data;
    //    node_t *node = list->head;
    //    list->head = list->head->next;
    //    return data;
    //} else {
    //    return NULL;
    //} 
    return list_rm_idx(list, 0);
}

/* Noted that, list_rmIdx do not free the memory of the removed node!
 */
void* list_rm_idx(list_t *list, int idx) {
    if(!list) {
        list->rc = ERR_NULL_POINTER;
        return NULL;
    }
    if(idx >= list->size || idx < 0) {
        list->rc = ERR_OUT_OF_RANGE; 
        return NULL;
    }

    // Locate index
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





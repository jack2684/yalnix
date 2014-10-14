#include "include/standardLib.h"
#include "stdlib.h"     // For the use of malloc, will be replace with yalnix malloc 

node_t *node_init(void* data) {
    node_t *node = (node_t*)malloc(sizeof(node_t));
    if(node) {
        node->data = data;
        return node;
    } else {
        return NULL;
    }
}


#include <stdio.h>
#include "../include/standardLib.h"
#include "../include/debug.h"

int main(int argc, char* argv[]) {
    int rc;
    int *data;
    node_t *node;
    list_t *list;
    
    printf("Start to test list...\n");

    printf("About to init\n");
    list = list_init();
    list_print(list);
    if(!list) {
        debug("Init list error\n");
        return -1;
    }

    printf("-------------------------------------\n");   
    data =  (int* )malloc(sizeof(int));
    *data = 1;
    node = node_init(data);
    if(!node) {
        debug("Cannot init node\n");
    }
    rc = list_pushFront(list, node);
    if(rc) {
        debug("List error code: %d\n", list->rc);
    }
    list_print(list);
    printf("\n");

    printf("-------------------------------------\n");   
    data =  (int* )malloc(sizeof(int));
    *data = 2;
    node = node_init(data);
    if(!node) {
        debug("Cannot init node\n");
    }
    rc = list_pushFront(list, node);
    list_print(list);
    printf("\n");
 
    printf("-------------------------------------\n");   
    data =  (int* )malloc(sizeof(int));
    *data = 3;
    node = node_init(data);
    if(!node) {
        debug("Cannot init node\n");
    }
    rc = list_pushFront(list, node);
    if(rc) {
        debug("List error code: %d\n", list->rc);
    }
    list_print(list);
    printf("\n");

    printf("-------------------------------------\n");   
    data =  (int* )malloc(sizeof(int));
    *data = 4;
    node = node_init(data);
    if(!node) {
        debug("Cannot init node\n");
    }
    rc = list_insert(list, node, 0);
    if(rc) {
        debug("List error code: %d\n", list->rc);
    }
    list_print(list);
    printf("\n");

    printf("-------------------------------------\n");   
    data =  (int* )malloc(sizeof(int));
    *data = 5;
    node = node_init(data);
    if(!node) {
        debug("Cannot init node\n");
    }
    rc = list_insert(list, node, list->size - 1);
    if(rc) {
        debug("List error code: %d\n", list->rc);
    }
    list_print(list);
    printf("\n");

    printf("-------------------------------------\n");   
    data =  (int* )malloc(sizeof(int));
    *data = 6;
    node = node_init(data);
    if(!node) {
        debug("Cannot init node\n");
    }
    rc = list_insert(list, node, list->size);
    if(rc) {
        debug("List error code: %d\n", list->rc);
    }
    list_print(list);
    printf("\n");

    printf("-------------------------------------\n");   
    rc = list_clear(list);
    if(rc && list) {
        debug("List error code: %d\n", list->rc);
    }
    list_print(list);
    printf("\n");

    printf("-------------------------------------\n");   
    data =  (int* )malloc(sizeof(int));
    *data = 7;
    node = node_init(data);
    if(!node) {
        debug("Cannot init node\n");
    }
    rc = list_insert(list, node, list->size);
    if(rc) {
        debug("List error code: %d\n", list->rc);
    }
    list_print(list);
    printf("\n");

    printf("-------------------------------------\n");   
    data =  (int* )malloc(sizeof(int));
    *data = 0;
    node = node_init(data);
    if(!node) {
        debug("Cannot init node\n");
    }
    rc = list_insert(list, node, list->size);
    if(rc) {
        debug("List error code: %d\n", list->rc);
    }
    list_print(list);
    printf("\n");

    printf("-------------------------------------\n");   
    rc = list_clear(list);
    if(rc && list) {
        debug("List error code: %d\n", list->rc);
    }
    list_print(list);
    printf("\n");

    printf("-------------------------------------\n");   
    data =  (int* )malloc(sizeof(int));
    *data = 8;
    node = node_init(data);
    if(!node) {
        debug("Cannot init node\n");
    }
    rc = list_pushRear(list, node);
    if(rc) {
        debug("List error code: %d\n", list->rc);
    }
    list_print(list);
    printf("\n");

    printf("-------------------------------------\n");   
    data =  (int* )malloc(sizeof(int));
    *data = 9;
    node = node_init(data);
    if(!node) {
        debug("Cannot init node\n");
    }
    rc = list_pushRear(list, node);
    if(rc) {
        debug("List error code: %d\n", list->rc);
    }
    list_print(list);
    printf("\n");

    printf("-------------------------------------\n");   
    data =  (int* )malloc(sizeof(int));
    *data = 10;
    node = node_init(data);
    if(!node) {
        debug("Cannot init node\n");
    }
    rc = list_insert(list, node, 0);
    if(rc) {
        debug("List error code: %d\n", list->rc);
    }
    list_print(list);
    printf("\n");

    printf("-------------------------------------\n");   
    data =  (int* )malloc(sizeof(int));
    *data = 11;
    node = node_init(data);
    if(!node) {
        debug("Cannot init node\n");
    }
    rc = list_insert(list, node, list->size);
    if(rc) {
        debug("List error code: %d\n", list->rc);
    }
    list_print(list);
    printf("\n");

    printf("-------------------------------------\n");   
    list_clear(list);
    free(list);
    list = NULL;
    if(list == NULL) {
        debug("list is empty!!\n");
    }
    if(rc) {
        debug("List error code: %d\n", rc);
    }
    list_print(list);
    printf("\n");
    
    printf("End list test successfully!\n");
}



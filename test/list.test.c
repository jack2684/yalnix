#include <stdio.h>
#include "../include/standard/list.h"

int main(int argc, char* argv[]) {

    printf("Start to test list...\n");

    list_t *list;
    printf("About to init\n");
    list = list_init();    

    printf("End list test successfully!\n");
}


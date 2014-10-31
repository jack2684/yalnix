/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#ifndef _VECTOR_H
#define _VECTOR_H
#include "common.h"

#define INIT_SIZE 64

typedef struct y_Vector {
    int size;
    int rc;
    void **datas;
} vector_t;
#endif

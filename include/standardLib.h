
/* Some useful library for both user and kernal
 */
#ifndef STANDARD_LIB
#define STANDARD_LIB

#include "standard/node.h"
#include "standard/list.h"
#include "standard/vector.h"

#define ERR_NULL_POINTER    1
#define ERR_OUT_OF_RANGE    2
#define ERR_NON_KNOWN       100

/* We don't need an explicit stack or queue, 
 * we can simply use vector or list as stack or queue
 */
//typedef struct y_Stack {
//    int         size;
//    void        *top;
//} stack_t;
//
//typedef struct y_Queue {
//    
//} queue_t;


typedef struct y_Heap {

} heap_t;

typedef struct y_Hashset {

} hashset_t;

typedef struct y_HashMap {

} hashmap_t;

#endif


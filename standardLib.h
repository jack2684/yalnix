
/* Some useful library for both user and kernal
 */
#ifndef STANDARD_LIB
#define STANDARD_LIB

#define ERR_NULL_POINTER    1
#define ERR_OUT_OF_RANGE    2
#define ERR_NON_KNOWN       100

typedef struct y_Node {
    void            *value;
    struct y_Node   *next;
} node_t;

typedef struct y_DoublyNode {
    void            *value;
    struct y_Node   *next;
    struct y_Node   *prev;
} dnode_t;

typedef struct y_List {
    int         size;
    int         rc;
    node_t      *front;    
    node_t      *rear;    
} list_t;

typedef struct y_DoublyList {
    int         size;
    int         rc;
    dnode_t     *front;    
    dnode_t     *rear;    
} dlist_t;

typedef struct y_Vector {
    int         size;
    int         rc;
    void        *arr;
} vector_t;

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

void *malloc(size_t size);
void free(void *ptr);
#endif


#ifndef _NODE_H
#define _NODE_H
typedef struct y_Node {
    void            *data;
    struct y_Node   *next;
} node_t;

node_t *node_init(void *data);

typedef struct y_DNode {
    void             *data;
    struct y_DNode   *next;
    struct y_DNode   *prev;
} dnode_t;

dnode_t *dnode_init(void *data);
#endif


#ifndef _NODE_H
#define _NODE_H
typedef struct y_Node {
    void            *data;
    struct y_Node   *next;
} node_t;

node_t *node_init(void *data);

struct y_DoublyNode {
    void            *data;
    struct y_Node   *next;
    struct y_Node   *prev;
};
#endif


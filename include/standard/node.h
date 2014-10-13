#ifndef _NODE_H
#define _NODE_H
typedef struct y_Node {
    void            *value;
    struct y_Node   *next;
} node_t;

struct y_DoublyNode {
    void            *value;
    struct y_Node   *next;
    struct y_Node   *prev;
};
#endif


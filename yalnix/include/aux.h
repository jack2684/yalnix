/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
/* This aux.h contains all those common helper funcitons
 */
#ifndef _AUX_H
#define _AUX_H

#include "common.h"
#include "inthashmap.h"
#include "dlist.h"

#define MAX_RESROUCE 1024

enum util_type {
    LOCK,
    PIPE,
    CVAR,
    NA,
};

typedef struct UTIL {
    void                *data;
    enum util_type       type;
} util_t;

extern hashmap_t *idp;      // The occupied id pool with corresponding util type
extern dlist_t *id_list;    // The available id pool

/* ID generator, to manage IDs like process id, pipe id, etc
 */
dlist_t *id_generator_init(int size);
int id_generator_pop(dlist_t *id_list);
int id_generator_push(dlist_t *id_list, int id);
int aux_get_next_id(dlist_t *id_list);

/* Utilities management
 */
int util_new_id();
int util_reclaim_id(int id);
int util_add(int id, void *data, enum util_type type);
int util_put(int id, void *data);
void *util_get(int id);
void *util_rm(int id);

#endif


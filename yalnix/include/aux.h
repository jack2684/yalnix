/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
/* This aux.h contains all those common helper funcitons
 */
#ifndef _AUX_H
#define _AUX_H

#include "common.h"
#include "dlist.h"

/* ID generator, to manage IDs like process id, pipe id, etc
 */
dlist_t *id_generator_init(int size);
int id_generator_pop(dlist_t *id_list);
int id_generator_push(dlist_t *id_list, int id);

#endif


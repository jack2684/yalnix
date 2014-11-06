/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
/* A very simple hash map, which only takes unsigned 32bit integer for key
 * 
 * The Bob Jenkins's hash algorithm implemenation in helper funciont do_hash(uint32 key)
 * is inspired from http://c.learncodethehardway.org/book/ex37.html
 */
#ifndef _INTHASHMAP_H
#define _INTHASHMAP_H

#include "hardware.h"
#include "yalnix.h"
#include "common.h"
#include "memory.h"
#include "dlist.h"

#define DEFAULT_SIZE    1024
#define MASK32          4294967294u // 0xFFFFFFFE, or you may say 0b11111111111111111111111111111110

typedef struct IHMAP {
    void    *datas[DEFAULT_SIZE];
    uint32  keys[DEFAULT_SIZE];
    int     capacity;
    int     size;
    int     rc;
} hashmap_t;

hashmap_t *hashmap_init();
int hashmap_clear(hashmap_t *hmap);
int hashmap_destroy(hashmap_t *hmap);
void *hashmap_get(hashmap_t *hmap, uint32 key);
int hashmap_put(hashmap_t *hmap, uint32 key, void *data);
int hashmap_rm(hashmap_t *hmap, uint32 key);

// Internal helper funciton
uint32 do_hash(uint32 key);
#endif


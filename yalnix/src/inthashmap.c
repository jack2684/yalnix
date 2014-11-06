#include "inthashmap.h"

hashmap_t *hashmap_init() {
    hashmap_t *hmap = (hashmap_t*)malloc(sizeof(hashmap_t));
    bzero(hmap, sizeof(hashmap_t));
    hmap->capacity = DEFAULT_SIZE;
    hmap->size = 0;
    return hmap;
}

void *hashmap_get(hashmap_t *hmap, uint32 key) {
    uint32 hash = do_hash(key);
    int i, idx;

    for(i = 0; i < hmap->capacity; i++) {
        idx = (hash % hmap->capacity + i) % hmap->capacity;
        if(hmap->keys[idx] == key) {
            return hmap->datas[idx];
        }
    }
   
    log_err("Key %u/%d not found in hashmap", key, key);
    return NULL;
}

int hashmap_put(hashmap_t *hmap, uint32 key, void *data) {
    uint32 hash = do_hash(key);
    int i, idx;

    if(hmap->size == hmap->capacity) {
        log_err("Hashmap is already full!!!", key, key);
        return 1;
    }

    for(i = 0; i < hmap->capacity; i++) {
        idx = (hash % hmap->capacity + i) % hmap->capacity;
        if(hmap->keys[idx] == key) {
            log_info("Hashmap override data at key %u/%d", key, key);
            hmap->datas[idx] = data;
            hmap->size++;
            return 0;
        } else if (hmap->datas[idx] == NULL) {
            hmap->datas[idx] = data;
            hmap->size++;
            return 0;
        }
    }

    log_err("Cannot put the data for key %u/%d", key, key);
    return 1;
}

int hashmap_rm(hashmap_t *hmap, uint32 key) {
    uint32 hash = do_hash(key);
    int i, idx;

    if(hmap->size == hmap->capacity) {
        log_err("Hashmap is already full!!!", key, key);
        return 1;
    }

    for(i = 0; i < hmap->capacity; i++) {
        idx = (hash % hmap->capacity + i) % hmap->capacity;
        if(hmap->keys[idx] == key) {
            hmap->datas[idx] = NULL;
            hmap->size--;
            return 0;
        }
    }

    log_err("Cannot put the data for key %u/%d", key, key);
    return 1;
}

int hashmap_destroy(hashmap_t *hmap) {
    free(hmap);
    return 0;
}

int hashmap_clear(hashmap_t *hmap) {
    int i = 0;
    for(i = 0; i < hmap->capacity; i++) {
        hmap->datas[i] = NULL;
        hmap->size--;
    }
    if(hmap->size != 0) {
        log_err("Something wrong with the size!!");
        return 1;
    }
    return 0;
}

uint32 do_hash(uint32 key) {
    int len = WORD_LEN;
    uint32 hash = 0;
    uint32 i = 0;

    for(i = 0; i < len; i++) {
        hash += ((key >> i) & MASK32);
        hash += (hash << 10);
        hash ^= (hash >> 6);
    } 

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}


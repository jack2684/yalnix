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
        if(hmap->keys[idx] == key && hmap->datas[idx] != NULL) {
            log_info("Find key %u, with data %p", key, hmap->datas[idx]);
            return hmap->datas[idx];
        }
    }
   
    log_err("Key %u/%d not found in hashmap", key, key);
    return NULL;
}

int hashmap_put(hashmap_t *hmap, uint32 key, void *data) {
    //log_info("Before do hash");
    uint32 hash = do_hash(key);
    //log_info("After do hash");
    int i, idx;

    if(hmap == NULL) {
        log_err("Hashmap pointer is NULL!");
        return 1;
    }
    if(hmap->size == hmap->capacity) {
        log_err("Hashmap is already full!!!", key, key);
        return 1;
    }

    //log_info("Adding key %d to hash map, with hash %u", key, hash);
    for(i = 0; i < hmap->capacity; i++) {
        idx = (hash % hmap->capacity + i) % hmap->capacity;
        if(hmap->datas[idx] == NULL) {
            //log_info("Hashmap add data at key %u/%d, data %p", key, key, data);
            hmap->datas[idx] = data;
            hmap->keys[idx] = key;
            hmap->size++;
            return 0;
        } else if (hmap->keys[idx] == key) {
            //log_info("Hashmap override data at key %u/%d", key, key);
            hmap->datas[idx] = data;
            return 0;
        }
    }

    log_err("Cannot put the data for key %u/%d", key, key);
    return 1;
}

void *hashmap_rm(hashmap_t *hmap, uint32 key) {
    uint32 hash = do_hash(key);
    int i, idx;
    void *data = NULL;

    if(hmap->size == hmap->capacity) {
        log_err("Hashmap is already full!!!", key, key);
        return NULL;
    }

    for(i = 0; i < hmap->capacity; i++) {
        idx = (hash % hmap->capacity + i) % hmap->capacity;
        if(hmap->keys[idx] == key) {
            data = hmap->datas[idx];
            hmap->datas[idx] = NULL;
            hmap->size--;
            return data;
        }
    }

    log_err("Cannot put the data for key %u/%d", key, key);
    return NULL;
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


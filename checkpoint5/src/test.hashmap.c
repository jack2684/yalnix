#include "inthashmap.h"

void main(void) {
    user_log("Testing hashmap");
    
    hashmap_t *hmap = hashmap_init();

    hashmap_print(hmap);

    int *d;
    int i;
    for(i = 0; i < 11; i++) {
        d = (int*)malloc(sizeof(int));
        *d = i * 10;
        hashmap_put(hmap, i, d);
    }
    hashmap_print(hmap);
}

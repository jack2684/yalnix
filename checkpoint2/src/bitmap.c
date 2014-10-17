#include "bitmap.h"

int get_word_number(uint32 idx) {
    return idx / WORD_LEN + ((idx % WORD_LEN) == 0 ? 0 : 1);
}

uint32 *init_bitmap(uint32 len) {
    uint32 *bitmap = (uint32*) malloc(sizeof(uint32) * get_word_number(len));
    if(!bitmap) {
        _debug("Cannot initiate bitmap\n");
        return NULL;
    }
    return bitmap;
}

int set_bit(uint32 *bitmap, uint32 idx, int bit) {
    int word_num = get_word_number(idx);
    int bit_offset = idx % WORD_LEN;
    uint32 setter = 1 << bit_offset;
    if(!bit) {  // Set to 0
        setter = ~setter;
        bitmap[word_num] = bitmap[word_num] & setter;
    } else {    // Set to 1
        bitmap[word_num] = bitmap[word_num] | setter;
    }
    return 0;
}

int set_bit_zero(uint32 *bitmap, uint32 idx) {
    return set_bit(bitmap, idx, 0);
}

int set_bit_one(uint32 *bitmap, uint32 idx) {
    return set_bit(bitmap, idx, 1);
}

int set_word_bits(uint32 *word, uint32 start, uint32 end, int bit) {
    if(end >= WORD_LEN || start > end) {
        _debug("Index out of range\n");
        return OUT_OF_RANGE_ERR;
    }

    uint32 len = end - start + 1;
    unsigned long long setter = 1;      // 00010000...
    setter = (setter << len) - 1;       // 00001111...
    sstter = setter << start;           // 00111...100
    if(!bit) {
        setter = ~setter;
        *word = *word & ((uint32)setter);
    } else {
        *word = *word | ((uint32)setter);
    }

    return 0;
}

int set_word_bits_zero(uint32 *word, uint32 start, uint32 end) {
    return set_word_bits(word, start, end, 0);
}

int set_word_bits_zero(uint32 *word, uint32 start, uint32 end) {
    return set_word_bits(word, start, end, 1);
}

int set_range(uint32 *bitmap, int start, int end, int bit) {
    int i;
    int start_num = get_word_number(start);
    int start_offset = start / WORD_LEN;
    int end_num = get_word_number(end);
    int end_offset = end / WORD_LEN;
    if(start_offset) {
        start_num++;
    }
    if(end_offset) {
        end_num--;
    }

    // Set bits in whole words
    for(i = start_num; i <= end_num; i++) {
        if(!bit) {
            bitmap[i] = 0;
        } else {
            bitmap[i] = ~((uint32)0);
        }
    }

    // Set bits in start and end word
    if(start_offset) {
        set_word_bits(bitmap + start_num - 1, start_offset, WORD_LEN, bit);
    }
    if(end_offset) {
        set_word_bits(bitmap + end_num - 1, end_offset, WORD_LEN, bit)
    }
}

int set_range_zero(uint32 *bitmap, int start, int end) {
    set_range(bitmap, start, end, 0);
}

int set_range_one(uint32 *bitmap, int start, int end) {
    set_range(bitmap, start, end, 1);
}

/* This part is a little tricky, I design this function with initial "_",
 * because I don't want to use malloc while doing the frame management.
 * So this function return 0 as illegal address.
 *
 * But normally, a bitmap should be legal to return 0
 */
int _get_and_flip_lowest_zero(uint32 *bitmap, int N) {
    int word_num = get_word_number(N);
    int offset = N % WORD_LEN;
    int i, idx;
    for(i = 0; i < word_num; i++) {
        idx = get_and_flip_lowest_zero_in_word(bitmap + i);
        if(idx >= 0) {
            return (uint32)idx + word_num * WORD_LEN;
        }
    }
    idx = get_and_flip_lowest_zero_in_word(bitmap + word_num);
    if(idx >= 0 && idx <= offset) {
        return (uint32)idx + word_num * WORD_LEN;    
    } else {
        return 0;       // Noted that, 0 is illegal here
    } 
}

int get_and_flip_lowest_zero_in_word(uint32 *word) {
    int idx;
}

int get_and_set_lowest_match(uint32 *bitmap, int match, int val) {
    
}


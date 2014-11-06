/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#ifndef _BV_H
#define _BV_H
#include "common.h"

#define OUT_OF_RANGE_ERR -1

/*
 *  The bits are aligned [high bits ... low bits]
 */

typedef struct y_bv {
    uint32 *bitmap;
    uint32 size;
} bitvec_t;

int get_word_number(uint32 idx);
int init_bitmap(uint32 len);

/* Set one bit in one word
 */
int set_bit(uint32 *bitmap, uint32 idx, int bit);
int set_bit_zero(uint32 *bitmap, uint32 idx);
int set_bit_one(uint32 *bitmap, uint32 idx);

/* Set multiple bits in one word
 */
int set_word_bits(uint32 *word, uint32 start, uint32 end, int bit);
int set_word_bits_zero(uint32 *word, uint32 start, uint32 end); 
int set_word_bits_one(uint32 *word, uint32 start, uint32 end); 


/* Set a range of bits, inclusive
 */
int set_range(uint32 *bitmap, int start, int end, int bit);
int set_range_zero(uint32 *bitmap, int start, int end);
int set_range_one(uint32 *bitmap, int start, int end);

/*  get set, range from 0 to N
 */
int get_and_set_lowest_match(uint32 *bitmap, int match, int val);
int get_and_flip_lowest_zero(uint32 *bitmap, N);
int get_and_flip_lowest_zero_in_word(uint32 *word);


int get_bit_in_word(uint32 word, uint32 idx);

#endif


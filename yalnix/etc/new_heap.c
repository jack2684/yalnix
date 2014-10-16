// real simple heap-management code
//
// s.w. smith, dartmouth college
// october 2004





//================================================

// globals
void *heap_start = 0;
void *heap_end   = 0;
u_long not_initialized = 1;

// for testing
void *fake_heap_start = 0;
void *fake_heap_end = 0;



//================================================

// the basic data structure
typedef struct mblock {
  u_long magic1;
  u_long size;  
  u_long size_used;
  struct mblock *next, *previous;
  u_long magic2;
} mblock_t;

//================================================

void init_heap();
mblock_t *grow_heap(u_long len);


// the first storage address in the block, given a ptr to the mblock struct
#define BLOCK_START(X)     ((void *)(((void *)(X)) + sizeof(mblock_t)))


// the first address where the NEXT block would start, 
// given a ptr to the mblock struct
#define BLOCK_END(X)       (BLOCK_START(X) + (X)->size)


// the first UNUSED address in the storage area, 
// given a ptr to the mblock struct
#define UBLOCK_END(X)      (BLOCK_START(X) + (X)->size_used)


// we use the magic number to help detect heap corruption
#define MAGIC 0xfeeddad  


// if we have a choice, don't bother creating heap block for 
// anything smaller than MINBLOCK
#define MINBLOCK (2 * sizeof(mblock_t))


// round to word len
#define WORDROUND(X)   ( (((u_long)(X)) + 3) & (~3) )



//================================================
// sanity check the block
// if there's a problem, print an error statement.
// but... always return.
void
sanity_check(mblock_t *bp) {


  if (NULL == bp) {
    TracePrintf(2,"HEAP Warning: sanity checking null heap block\n");
    return;
  }


  if ( (((void *)bp) < heap_start)  || ( ((void *)bp) >= heap_end) ) 
    TracePrintf(2,"HEAP Warning: heap block at 0x%x lies outside heap [0x%x -> 0x%x]\n",bp, heap_start,heap_end);


  if ( (bp->magic1 != MAGIC) || (bp->magic2 != MAGIC) )
    TracePrintf(2,"HEAP Warning: heap block has corrupted magic number\n");

  if (bp->size_used > bp->size)
    TracePrintf(2,"HEAP Warning: heap block is overallocated\n");


  if (bp->next) {
    if (  BLOCK_END(bp)  !=  (void *)(bp->next) )
      TracePrintf(2,"HEAP Warning: next heap block is not adjacent\n");
  }

}


void
check_heap(void) {

  mblock_t *bp;
  u_long 
    blocks = 0, 
    dirty = 0,
    free_block_bytes = 0, 
    used_block_bytes = 0, 
    ifrag_bytes = 0;

  TracePrintf(2,"HEAP check_heap()\n");

  if (not_initialized) {
    TracePrintf(2,"HEAP heap not initialized\n");
    if (USER_LIB)
      init_heap();
    else return;
  }


  TracePrintf(2,"HEAP total size = 0x%x\n", heap_end - heap_start);

  for (bp = (mblock_t *)heap_start; bp; bp = bp->next) {
    sanity_check(bp);
    blocks++;
    if (bp->size_used) {
      dirty++;
      used_block_bytes += bp->size_used;
      ifrag_bytes += bp->size - bp->size_used;
    } else {
      free_block_bytes += bp->size;
    }
  
  }

  TracePrintf(2,"HEAP Blocks: 0x%x (0x%x dirty)\n", blocks,dirty);
  TracePrintf(2,"HEAP Bytes: 0x%x free, 0x%x used, 0x%x ifrag\n",
	  free_block_bytes, used_block_bytes, ifrag_bytes);


  TracePrintf(2,"HEAP done\n");

}


//================================================
// assumption: bp is free, and its next block exists and is free too.
// action: we merge these two blocks into one block

// return address of bp, if we merged; o.w. return bp->next
// this makes garbage collection easier

mblock_t *
merge_free_pair(mblock_t *bp) {


  mblock_t *first, *second;
  

  first = bp;

  if (NULL == first)
    return (NULL);

  second = bp->next;
  if (NULL == second)
    return (NULL);

  sanity_check(first);
  sanity_check(second);

  if (first->size_used)
    return (second);

  if (second->size_used)
    return (second);

  // ok, both blocks exist and are indeed free.


  // absorb the bytes
  first->size = first->size + second->size + sizeof(mblock_t);

  // link it
  first->next = second->next;
  if (second->next) {
    sanity_check(second->next);
    second->next->previous = first;
  }

  // yee-haw! we're done
  return (first);
}		     


//==============================================
// assumption: bp is a big empty block, with at least len bytes 
// action: we'd like to split it into two.  the first one should be able
// to hold at least len bytes.  
// when we're done, bp can hold len bytes.

void
split_free_block(mblock_t *bp, u_long len) {

  mblock_t  *second;
  
  len = WORDROUND(len);

  sanity_check(bp);

  if (bp->size_used) {
    TracePrintf(2, "heap problem: splitting nonempty block\n");
    return;
  }

  if (bp->size < len) {
    TracePrintf(2, "heap problem: splitting block that's too small\n");
    return;
  }

  // don't bother splitting
  if (bp->size < (len + MINBLOCK) )
    return;

  //ok, split it off

  second = (mblock_t *)(  ((void *)bp) + sizeof(mblock_t) + len );
  second->magic1 = MAGIC;
  second->size = bp->size - len - sizeof(mblock_t);
  bp->size = len;
  second->size_used = 0;
  second->previous = bp;
  second->next = bp->next;
  bp->next = second;
  if (second->next)
    second->next->previous = second;
  second->magic2 = MAGIC;
}


//====================
void
garbage_collect(void) {

  mblock_t *bp,*np;;

  bp = ((mblock_t *) heap_start); 

  while(bp) {
    np = merge_free_pair(bp);
    bp = np;
  }
}

//==============================

void
free(void *addr) {


  mblock_t *bp;

  TracePrintf(2,"HEAP my_free(0x%x)\n", addr);

  if (NULL == addr)
    return;

  if (not_initialized) {
    TracePrintf(2, "warning: unitialized heap!\n");
    if (USER_LIB)
      init_heap();
    else return;
  }



  for (bp = ((mblock_t *) heap_start); bp; bp = bp->next) {
    sanity_check(bp);

    if ( (BLOCK_START(bp) <= addr) && (addr < BLOCK_END(bp)) ) {

      TracePrintf(2,"HEAP found the block\n");
      
      if ( 0 == bp->size_used) {
	TracePrintf(2,"HEAP Warning: block was already free\n");
	return;
      }

      // mark this block as free
      bp->size_used = 0;

      // garbage collect
      garbage_collect();
      return;
    }
  }

  TracePrintf(2,"HEAP Warning: could not find the block\n");

}

//========================
void *
malloc(size_t len) {


  mblock_t *bp, *last;


  TracePrintf(2,"HEAP my_malloc(0x%x)\n", len);

  len = WORDROUND(len);

  if (not_initialized) {
    TracePrintf(2, "warning: unitialized heap!\n");
    if (USER_LIB)
      init_heap();
    else  return NULL;
  }

  for (bp = (mblock_t *)heap_start; bp; bp = bp->next) {
    
    sanity_check(bp);
    if ( (0 == bp->size_used) && (len <= bp->size) ) {
      TracePrintf(2,"HEAP found a block\n");
      split_free_block(bp,len);
      bp->size_used = len;
      return BLOCK_START(bp);
    }

    last = bp;

  }

  TracePrintf(2,"HEAP not enough space, so going to try to grow\n");

  if (NULL == last) {
    TracePrintf(2,"HEAP Warning: no last block, oddly\n");
    return NULL;
  }

  // grow heap
  bp = grow_heap(len);

  if (NULL == bp) {
    TracePrintf(2,"HEAP Warning: grow heap failed\n");
    return NULL;
  }

  last->next = bp;
  bp->previous = last;

  // merge the new block with the last one, in case the last one had been
  // free tpp
  bp = merge_free_pair(last);

  if ( (NULL == bp) || (bp->size_used) || (bp->size < len) ) {
    TracePrintf(2,"HEAP Warning: something odd happened with the merge\n");
    return NULL;
  }

  split_free_block(bp,len);
  bp->size_used = len;
  return BLOCK_START(bp);

}



void *calloc(size_t nmemb, size_t size) {

  void *rp;


  TracePrintf(2,"HEAP new heap calloc\n");
  rp = malloc (nmemb * size);

  if (rp)
    memset(rp, 0x00, nmemb*size);

  return rp;

}


void *realloc(void *ptr, size_t size) {


  void *rp;
  mblock_t *bp,*np;
  size_t copied_size;
  

  TracePrintf(2,"HEAP new heap realloc\n");


  if (0 == size) {
    free(ptr);
    return NULL;
  }

  if (0 == ptr) {
    return (malloc(size));
  }


  // not the best implementaiton


  // find the old block

  if (not_initialized) {
    TracePrintf(2, "warning: unitialized heap!\n");
    if (USER_LIB)
      init_heap();
    else return NULL;
  }

  
  for (bp = ((mblock_t *) heap_start); bp; bp = bp->next) {
    sanity_check(bp);

    if (BLOCK_START(bp) == ptr) 
      break;
  }
  
  if (NULL == bp) {
    TracePrintf(2,"HEAP Warning: could not find the block\n");
    return NULL;
  }



  // easy case: can we fit within this block?
  if (size  <=  bp->size ) {
    bp->size_used = size;
    return (ptr);
  }

  // hard case: we have to grow

  // can we do it in place?
  np = bp->next;
  if (np && (0 == np->size_used) && (size <= bp->size + np->size + sizeof(mblock_t))) {
    
    bp->next = np->next;
    bp->size = bp->size + np->size + sizeof(mblock_t);
    bp->size_used = size;
    if (np->next) 
      np->next->previous = bp;

    return ptr;
  }


  // ok, we have to do it the hard way!


  rp = malloc(size);
  if (NULL == rp)
    return NULL;


  memcpy(rp,ptr,bp->size_used);

  free(ptr);

  return rp;

}

#include "common.h"
#include "memory.h"
#include "list.h"

pte_t   *kernel_page_table = NULL;          // Page table for kernel
pte_t   *user_page_table = NULL;            // Page table for kernel
list_t  *available_frames = NULL;           // For physcial memories
vm_t    kernel_memory;                      // Kernel virtual memories
vm_t    user_memory;                        // User virtual memories
uint32  PAGE_SIZE;

/* Init a frame strcut with frame number
 */
frame_t *init_frame(uint32 idx) {
    frame_t *f = (uint32*) malloc(sizeof(frame_t));
    if(!f) {
        _debug("Cannot create new frame via malloc\n");
        return NULL;
    }
    *f = (frame_t)idx;
    return f;
}

/* Get the actual frame number from frame struct
 */
uint32 frame_get_pfn(frame_t* f) {
    return (uint32)(*f);
}

/* Add available frame to frame list
 *
 * @param pfn: the page frame number
 */
int add_tail_available_frame(uint32 pfn) {
    int rc;
    frame_t *frame = init_frame(pfn);
    
    node_t *n = list_add_tail(available_frames, (void*)frame);
    if(!n) {
        _debug("Cannot add more free frame\n");
        return MALLOC_ERR;
    }

    return 0;
}

/* Remove the first available frame in the frame list
 */
frame_t *rm_head_available_frame() {
    if(list_is_empty(available_frames) == 1) {
        _debug("Frame list is empty, double check size: %d!\n", available_frames->size);
        return NULL;
    }

    frame_t *frame = (frame_t*)list_rm_head(available_frames);
    if(!frame) {
        _debug("list_rm_head error code: %d\n", available_frames->rc);
    }
    return frame;
}

/* Flush the TLB
 */
void flush_region_TLB(pte_t* table) {
    if(table == kernel_page_table) {
        WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_0);
    } else {
        WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_1);
    }
}

/* Map pages to frames, for each pte to be mapped, 
 * get a free frame from the available frame list
 *
 * @param this_page_table: pointer to a page table 
 * @param start_idx: start index in the page table
 * @param end_idx: end index in the page table (exclusive)
 * @param prot: page protection
 * @return: whether the mapping is successful
 */
int map_page_to_frame(pte_t* this_page_table, int start_page_idx, int end_page_idx, int prot) {
    int rc = 0, i;

    // Try mapping
    if(this_page_table == kernel_page_table) {
        log_info("Map from page %d (%p) to page %d (%p)", start_page_idx, KERNEL_PAGE_TO_ADDR(start_page_idx), end_page_idx, KERNEL_PAGE_TO_ADDR(end_page_idx));
    } else {
        log_info("Map from page %d (%p) to page %d (%p)", start_page_idx, USER_PAGE_TO_ADDR(start_page_idx), end_page_idx, USER_PAGE_TO_ADDR(end_page_idx));
    }
    for(i = start_page_idx; i < end_page_idx && !rc; i++) {
        if(this_page_table[i].valid == _VALID) {
            _debug("Page %d is valid already with prot %d\n", i, this_page_table[i].prot);
            continue;
        }
        frame_t *frame = rm_head_available_frame();
        if(!frame) {
            _debug("Page %d cannot find available frame\n", i);
            rc = NO_AVAILABLE_ERR;
        } else {
            this_page_table[i].valid = _VALID;
            this_page_table[i].prot = prot;
            this_page_table[i].pfn = frame_get_pfn(frame);
        }
    }

    // Flush the toilet!
    flush_region_TLB(this_page_table);
    return rc;
}

/* Set pages prot
 *
 * @param this_page_table: pointer to a page table 
 * @param start_idx: start index in the page table
 * @param end_idx: end index in the page table (exclusive)
 * @param prot: page protection
 * @return: whether the setting is successful
 */
int set_ptes(pte_t* this_page_table, int start_page_idx, int end_page_idx, int prot) {
    int rc = 0, i;

    _debug("Set from page %d (%p) to page %d (%p)\n", start_page_idx, USER_PAGE_TO_ADDR(start_page_idx), end_page_idx, USER_PAGE_TO_ADDR(end_page_idx));
    for(i = start_page_idx; i < end_page_idx; i++ ) {
        this_page_table[i].prot = prot;
        if(this_page_table == kernel_page_table) {
            WriteRegister(REG_TLB_FLUSH, KERNEL_PAGE_TO_ADDR(i));
        } else {
            WriteRegister(REG_TLB_FLUSH, USER_PAGE_TO_ADDR(i));
        }
    }
    flush_region_TLB(this_page_table);
    return rc;
}

/* Unmap pages to frames
 *
 * @param this_page_table: pointer to a page table 
 * @param start_idx: start index in the page table
 * @param end_idx: end index in the page table (exclusive)
 * @return: whether the unmapping is successful
 */
int unmap_page_to_frame(pte_t* this_page_table, int start_page_idx, int end_page_idx) {
    int rc = 0, i;
   
    // Try unmapping
    log_info("About to unmap from %d to %d", start_page_idx, end_page_idx);
    for(i = start_page_idx; i < end_page_idx && !rc; i++) {
        //_debug("unmapping idx %d to %p with pte at %p\n", i, frame_get_pfn(frame), this_page_table + i);
        if(this_page_table[i].valid == _INVALID) {
            continue;
        }
        this_page_table[i].valid = _INVALID;
        rc = add_tail_available_frame(this_page_table[i].pfn);
        if(rc) {
            _debug("Cannot add new frame when trying to unmap\n");
            rc = MALLOC_ERR;
        }
    }

    // Flush the toilet!
    flush_region_TLB(this_page_table);
    return rc;
}


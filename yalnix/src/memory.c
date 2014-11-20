/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#include "common.h"
#include "memory.h"
#include "list.h"

pte_t   *kernel_page_table = NULL;          // Page table for kernel
pte_t   *user_page_table = NULL;            // Page table for kernel
dlist_t *available_frames = NULL;           // For physcial memories
vm_t    kernel_memory;                      // Kernel virtual memories
vm_t    user_memory;                        // User virtual memories
uint32  total_page_number;

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
    
    dnode_t *n = dlist_add_tail(available_frames, (void*)frame);
    if(!n) {
        _debug("Cannot add more free frame\n");
        return MALLOC_ERR;
    }

    return 0;
}

/* Allocate (end_idx - start_idx) frames, and copy data from current page table.
 * This part is tricky, because the next page table is not in Virtual Memory yet,
 * we borrow the Swap page in VMEM 0 to swap all those data from current page table frames.
 *
 * @dest_table: the next page table to copy to
 * @src_table: the current page table of VMEM 0 or VMEM 1
 * @start_idx: start idx
 * @end_idx: end idx, exclusive
 * @swap_addr: the special addr in VMEM 0
 * @return: whether it is successful or not
 */
int alloc_frame_and_copy(pte_t *dest_table, pte_t *src_table, int start_idx, int end_idx, uint32 swap_addr) {
    int swap_idx = KERNEL_PAGE_NUMBER(swap_addr);
    pte_t *swap_pte = (kernel_page_table + swap_idx);
    uint32 src_data_addr;
    int i, rc = 0;

    if(src_table == kernel_page_table) {
        //log_info("Going to copy from %d(%p) to %d(%p)", start_idx, KERNEL_PAGE_TO_ADDR(start_idx), end_idx, KERNEL_PAGE_TO_ADDR(end_idx));
    } else {
        //log_info("Going to copy from %d(%p) to %d(%p)", start_idx, USER_PAGE_TO_ADDR(start_idx), end_idx, USER_PAGE_TO_ADDR(end_idx));
    } 
   
    for(i = start_idx; i < end_idx && !rc; i++) {
        if(src_table[i].valid != _VALID) {
            dest_table[i - start_idx].valid = _INVALID;
            continue;
        }
        frame_t *frame = rm_head_available_frame();
        if(!frame) {
            log_err("Page %d cannot find available frame", i);
            unmap_page_to_frame(dest_table, start_idx, i);
            rc = NO_AVAILABLE_ERR;
        } else {
            // swap_pte exit in valid virtual memory, 
            // so after copying original data from src_data_addr,
            // swap_pte give this frame to the newly created pte
            swap_pte->valid = _VALID;
            swap_pte->prot = PROT_ALL;              // Set prot for copy purpose
            swap_pte->pfn = frame_get_pfn(frame);
            
            // Copying
            if(src_table == kernel_page_table) {
                src_data_addr = KERNEL_PAGE_TO_ADDR(i);
            } else {
                src_data_addr = USER_PAGE_TO_ADDR(i);
            } 
            WriteRegister(REG_TLB_FLUSH, swap_addr);
            //log_info("Copy data to phy frame %d", swap_pte->pfn);
            memcpy((void*)swap_addr, (void*)src_data_addr, PAGESIZE);
            dest_table[i - start_idx] = *swap_pte;
            dest_table[i - start_idx].prot = src_table[i].prot;     //Restore the prot
        }
    }
    swap_pte->valid = _INVALID;

    if(rc) {
        log_err("alloc_frame_and_copy fail");
    }
    return rc;
}

/* Remove the first available frame in the frame list
 */
frame_t *rm_head_available_frame() {
    if(dlist_is_empty(available_frames) == 1) {
        log_err("Frame list is empty, double check size: %d!", available_frames->size);
        return NULL;
    }

    frame_t *frame = (frame_t*)dlist_rm_head(available_frames);
    if(!frame) {
        log_err("list_rm_head error code: %d\n", available_frames->rc);
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
 * @param page_table: pointer to a page table 
 * @param start_idx: start index in the page table
 * @param end_idx: end index in the page table (exclusive)
 * @param prot: page protection
 * @return: whether the mapping is successful
 */
int map_page_to_frame(pte_t* page_table, int start_idx, int end_idx, int prot) {
    int rc = 0, i;

    // Try mapping
    if(page_table == kernel_page_table) {
        log_info("Map from page %d (%p) to page %d (%p)", start_idx, KERNEL_PAGE_TO_ADDR(start_idx), end_idx, KERNEL_PAGE_TO_ADDR(end_idx));
    } else {
        log_info("Map from page %d (%p) to page %d (%p)", start_idx, USER_PAGE_TO_ADDR(start_idx), end_idx, USER_PAGE_TO_ADDR(end_idx));
    }
    for(i = start_idx; i < end_idx && !rc; i++) {
        if(page_table[i].valid == _VALID) {
            log_info("Page %d is valid already with prot %d and pfn %d", i, page_table[i].prot, page_table[i].pfn);
            continue;
        }
        frame_t *frame = rm_head_available_frame();
        if(!frame) {
            log_err("Page %d cannot find available frame", i);
            unmap_page_to_frame(page_table, start_idx, i);
            rc = NO_AVAILABLE_ERR;
        } else {
            page_table[i].valid = _VALID;
            page_table[i].prot = prot;
            page_table[i].pfn = frame_get_pfn(frame);
            //log_info("Map pte %d=>%d", i, page_table[i].pfn);
        }
    }

    // Flush the toilet!
    flush_region_TLB(page_table);
    return rc;
}

/* Set pages prot
 *
 * @param page_table: pointer to a page table 
 * @param start_idx: start index in the page table
 * @param end_idx: end index in the page table (exclusive)
 * @param prot: page protection
 * @return: whether the setting is successful
 */
int set_ptes(pte_t* page_table, int start_idx, int end_idx, int prot) {
    int rc = 0, i;

    log_info("Set from page %d (%p) to page %d (%p)", start_idx, USER_PAGE_TO_ADDR(start_idx), end_idx, USER_PAGE_TO_ADDR(end_idx));
    for(i = start_idx; i < end_idx; i++ ) {
        page_table[i].prot = prot;
        if(page_table == kernel_page_table) {
            WriteRegister(REG_TLB_FLUSH, KERNEL_PAGE_TO_ADDR(i));
        } else {
            WriteRegister(REG_TLB_FLUSH, USER_PAGE_TO_ADDR(i));
        }
    }
    flush_region_TLB(page_table);
    return rc;
}

/* Unmap pages to frames
 *
 * @param page_table: pointer to a page table 
 * @param start_idx: start index in the page table
 * @param end_idx: end index in the page table (exclusive)
 * @return: whether the unmapping is successful
 */
int unmap_page_to_frame(pte_t* page_table, int start_idx, int end_idx) {
    int rc = 0, i;

    log_info("About to unmap from %d to %d", start_idx, end_idx);
    if(start_idx >= end_idx) {
        return 0;
    }
   
    // Try unmapping
    for(i = start_idx; i < end_idx && !rc; i++) {
        //_debug("unmapping idx %d to %p with pte at %p\n", i, frame_get_pfn(frame), page_table + i);
        if(page_table[i].valid == _INVALID) {
            continue;
        }
        page_table[i].valid = _INVALID;
        rc = add_tail_available_frame(page_table[i].pfn);
        if(rc) {
            log_err("Cannot add new frame when trying to unmap");
            rc = MALLOC_ERR;
        }
    }

    // Flush the toilet!
    flush_region_TLB(page_table);
    return rc;
}

/* Set global user page table to a new one
 *
 * @param page_table: the new page table to be set
 */
void set_user_page_table(pte_t* page_table) {
    if(!page_table) {
        log_err("The pcb user page table is NULL");
    }
    user_page_table = page_table;
    WriteRegister(REG_PTBR1, (uint32)user_page_table);
    WriteRegister(REG_PTLR1, GET_PAGE_NUMBER(VMEM_1_SIZE));
    WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_1);
}

void print_page_table(pte_t *pt, int s, int e) {
    int i = 0;
    if(pt == kernel_page_table) {
        log_info("PRINT KERNEL PAGE TABLE>>>>>>>>>>>>");
    } else {
        log_info("PRINT USER PAGE TABLE>>>>>>>>>>>>");
    }
    for(i = s; i < e; i ++ ) {
        log_info("pte[%d]: valid=>%d\tprot=>%d\tpfn=>%d", i, pt[i].valid, pt[i].prot, pt[i].pfn);
    }
    log_info("<<<<<<<<<<<<<<<PRINT TABLE DONE");
}

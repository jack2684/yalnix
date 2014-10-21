#include "common.h"
#include "memory.h"
#include "list.h"

pte_t   *kernel_page_table = NULL;          // Page table for kernel
pte_t   *user_page_table = NULL;            // Page table for kernel
list_t  *available_frames = NULL;           // For physcial memories
vm_t    kernel_memory;                      // Kernel virtual memories
uint32  PAGE_SIZE;

frame_t *init_frame(uint32 idx) {
    frame_t *f = (uint32*) malloc(sizeof(frame_t));
    if(!f) {
        _debug("Cannot create new frame via malloc\n");
        return NULL;
    }
    *f = (frame_t)idx;
    return f;
}

uint32 frame_get_pfn(frame_t* f) {
    return (uint32)(*f);
}

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

void flush_region_TLB(pte_t* table) {
    if(table == kernel_page_table) {
        WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_0);
    } else {
        WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_1);
    }
}

int map_page_to_frame(pte_t* this_page_table, int start_page_idx, int page_cnt, int prot) {
    int end_page_idx = start_page_idx + page_cnt;
    int rc = 0, i;

    _debug("Start to map from page %d to page %d\n", start_page_idx, end_page_idx);
    // Try mapping
    for(i = start_page_idx; i < end_page_idx && !rc; i++) {
        frame_t *frame = rm_head_available_frame();
        if(!frame) {
            _debug("No more available frame\n");
            rc = NO_AVAILABLE_ERR;
        } else {
            _debug("Map pfn: %d\n", frame_get_pfn(frame));
            this_page_table[i].valid = _VALID;
            _debug("Map set valid: %d\n", frame_get_pfn(frame));
            this_page_table[i].prot = prot;
            _debug("Map set prot: %d\n", frame_get_pfn(frame));
            this_page_table[i].pfn = frame_get_pfn(frame);
            _debug("Map set pfn: %d\n", frame_get_pfn(frame));
        }
    }

    _debug("Map done\n");
    // Flush the toilet!
    flush_region_TLB(this_page_table);
    return rc;
}

int set_ptes(pte_t* this_page_table, int start_page_idx, int page_cnt, int prot) {
    int end_page_idx = start_page_idx + page_cnt;
    int rc = 0, i;

    _debug("Start to set from page %d to page %d\n", start_page_idx, end_page_idx);
    for(i = start_page_idx; i < end_page_idx; i++ ) {
        this_page_table[i].prot = prot;
    }
    _debug("Map done\n");
}

int unmap_page_to_frame(pte_t* this_page_table, int start_page_idx, int page_cnt) {
    int end_page_idx = start_page_idx + page_cnt;
    int rc = 0, i;
   
    // Try unmapping
    for(i = start_page_idx; i < end_page_idx && !rc; i++) {
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

int Y_Brk(void * addr){
	//IF new break space impose on the  current user stack space
		//REPORT ERROR
	//END IF

	//IF new break segment is larger than current user break segment
		//ENLARGE the user heap
	//END IF

	//IF new break segment is smaller than the current user break segment
		//SHRINK the heap space
	//END IF
}

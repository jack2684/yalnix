/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
#ifndef _MEMORY_H
#define _MEMORY_H
#include "hardware.h"
#include "dlist.h"
#include "common.h"

#define GET_PAGE_NUMBER(addr) (((uint32)addr) >> PAGESHIFT)
#define KERNEL_PAGE_NUMBER(addr) (((uint32)addr) >> PAGESHIFT)
#define USER_PAGE_NUMBER(addr) (((uint32)addr - VMEM_1_BASE) >> PAGESHIFT)
#define GET_PAGE_FLOOR_NUMBER(addr) (DOWN_TO_PAGE((uint32)addr) >> PAGESHIFT)
#define GET_PAGE_CEILING_NUMBER(addr) (UP_TO_PAGE((uint32)addr) >> PAGESHIFT)
#define GET_PAGE_OFFSET(addr) (addr & PAGEMASK)
#define KERNEL_PAGE_TO_ADDR(idx) (idx << PAGESHIFT)
#define USER_PAGE_TO_ADDR(idx) ((idx << PAGESHIFT) + VMEM_1_BASE)

#define MALLOC_ERR          1
#define NO_AVAILABLE_ERR    2

// Some globals
typedef struct      pte pte_t;
typedef uint32      frame_t;
extern pte_t        *kernel_page_table;                // Global for every one
extern pte_t *user_page_table;
extern uint32       total_page_number;
extern dlist_t      *available_frames;

// Memory management of virtual memory
typedef struct y_vm {
   uint32 text_low, text_high;
   uint32 data_low, data_high;      
   uint32 env_low, env_high;         
   uint32 brk_low, brk_high;
   uint32 stack_low, stack_high;     
   uint32 swap_addr;                    // The special addr for physical memory copy
   uint32 size;                         // Size of a process memory space
} vm_t;

// Some globals
extern vm_t kernel_memory;
extern vm_t user_memory;

frame_t *init_frame(uint32 idx);
uint32 frame_get_pfn(frame_t* f);
int add_tail_available_frame(uint32 pfn);
frame_t *rm_head_available_frame();
void flush_region_TLB(pte_t* table);
int map_page_to_frame(pte_t* page_table, int start_page_idx, int end_page_idx, int prot);
int set_ptes(pte_t* page_table, int start_page_idx, int end_page_idx, int prot);
int unmap_page_to_frame(pte_t* page_table, int start_page_idx, int end_page_idx);
int init_page_table_from(pte_t* new_page_table, pte_t* old_page_table);
void print_page_table(pte_t *pt, int s, int e);

#endif


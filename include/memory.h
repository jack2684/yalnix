#ifndef MEMORY_H
#define MEMORY_H
// Page struct
struct y_page {
    unsigned long   base;
    unsigned long   offset;
    struct y_page   *next;
    struct y_page   *prev;
};

// Memory management
struct y_mm {
//   struct y_VMA     *vma_list;                  // List of VMAs ??
   unsigned long    code_start, code_end;
   unsigned long    data_start, data_end;      
   unsigned long    env_start, env_end;         
   unsigned long    brk_start, brk_end;
   unsigned long    stack_start, stack_end;     
   unsigned long    size;                       // Size of a process memory space
};
#endif


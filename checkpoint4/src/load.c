/* Team 3: stderr, Junjie Guan, Ziyang Wang*/
//==>> This is a TEMPLATE for how to write your own LoadProgram function.
//==>> Places where you must change this file to work with your kernel are
//==>> marked with "==>>".  You must replace these lines with your own code.
//==>> You might also want to save the original annotations as comments.

#include <fcntl.h>
#include <unistd.h>
#include <hardware.h>
#include <load_info.h>

// ==>> #include anything you need for your kernel here

#include "proc.h"
#include "memory.h"
#include "load.h"
#include "common.h"

/*
 *  Load a program into an existing address space.  The program comes from
 *  the Linux file named "name", and its arguments come from the array at
 *  "args", which is in standard argv format.  The argument "proc" points
 *  to the process or PCB structure for the process into which the program
 *  is to be loaded. 
 */
int LoadProgram(char *name, char *args[], pcb_t *proc) 
//==>> Declare the argument "proc" to be a pointer to your PCB or
//==>> process descriptor data structure.  We assume you have a member
//==>> of this structure used to hold the cpu context 
//==>> for the process holding the new program.  
{
    int fd;
    int (*entry)();
    struct load_info li;
    int i;
    char *cp;
    char **cpp;
    char *cp2;
    int argcount;
    int size;
    int text_pg1;
    int data_pg1;
    int data_npg;
    int stack_npg;
    long segment_size;
    char *argbuf;

    log_info("Load program: name => %s", name);
  
  /*
   * Open the executable file 
   */
    if ((fd = open(name, O_RDONLY)) < 0) {
        log_err("Can't open file '%s'", name);
        return _FAILURE;
    }

    if (LoadInfo(fd, &li) != LI_NO_ERROR) {
        log_err("'%s' not in Yalnix format", name);
        close(fd);
        return (-1);
    }

    if (li.entry < VMEM_1_BASE) {
        log_err("'%s' not linked for Yalnix", name);
        close(fd);
        return _FAILURE;
    }

    log_info("Open file DONE.");
  /*
   * Figure out in what region 1 page the different program sections
   * start and end
   */
    text_pg1 = (li.t_vaddr - VMEM_1_BASE) >> PAGESHIFT;
    data_pg1 = (li.id_vaddr - VMEM_1_BASE) >> PAGESHIFT;
    data_npg = li.id_npg + li.ud_npg;
  /*
   *  Figure out how many bytes are needed to hold the arguments on
   *  the new stack that we are building.  Also count the number of
   *  arguments, to become the argc that the new "main" gets called with.
   */
    log_info("Set boundaries done, about to calculate the args");
    size = 0;
    for (i = 0; args[i] != NULL; i++) {
        log_info("counting arg %d = '%s'", i, args[i]);
        log_info("counting arg %d = '%s'\n", i, args[i]);
        size += strlen(args[i]) + 1;
    }
    argcount = i;

    log_info("argsize %d, argcount %d", size, argcount);
  
  /*
   *  The arguments will get copied starting at "cp", and the argv
   *  pointers to the arguments (and the argc value) will get built
   *  starting at "cpp".  The value for "cpp" is computed by subtracting
   *  off space for the number of arguments (plus 3, for the argc value,
   *  a NULL pointer terminating the argv pointers, and a NULL pointer
   *  terminating the envp pointers) times the size of each,
   *  and then rounding the value *down* to a double-word boundary.
   */
    cp = ((char *)VMEM_1_LIMIT) - size;

    cpp = (char **)
    (((int)cp - 
      ((argcount + 3 + POST_ARGV_NULL_SPACE) *sizeof (void *))) 
     & ~7);

  /*
   * Compute the new stack pointer, leaving INITIAL_STACK_FRAME_SIZE bytes
   * reserved above the stack pointer, before the arguments.
   */
    cp2 = (caddr_t)cpp - INITIAL_STACK_FRAME_SIZE;



    log_info("prog_size %d, text %d data %d bss %d pages",
      li.t_npg + data_npg, li.t_npg, li.id_npg, li.ud_npg);


  /* 
   * Compute how many pages we need for the stack */
    stack_npg = (VMEM_1_LIMIT - DOWN_TO_PAGE(cp2)) >> PAGESHIFT;

    log_info("heap_size %d, stack_size %d",
	            li.t_npg + data_npg, stack_npg);
    log_info("heap_size %d, stack_size %d",
                     li.t_npg + data_npg, stack_npg);


    /* leave at least one page between heap and stack */
    if (stack_npg + data_pg1 + data_npg >= MAX_PT_LEN) {
        close(fd);
        return _FAILURE;
    }

    /*
     * This completes all the checks before we proceed to actually load
     * the new program.    From this point on, we are committed to either
     * loading succesfully or killing the process.
     */

    /*
     * Set the new stack pointer value in the process's exception frame.
     */

// ==>> Here you replace your data structure proc
// ==>> proc->context.sp = cp2;
    proc->user_context.sp = cp2; // @TODO: user context or kernel context?
    ("proc->user_context sp: %p", proc->user_context.sp);

    /*
     * Now save the arguments in a separate buffer in region 0, since
     * we are about to blow away all of region 1.
     */
    cp2 = argbuf = (char *)malloc(size);
// ==>> You should perhaps check that malloc returned valid space
    if(cp2 == NULL) {
        log_info("Malloc for cp2/argbuf error!");
    }

    for (i = 0; args[i] != NULL; i++) {
        log_info("saving arg %d = '%s'", i, args[i]);
        strcpy(cp2, args[i]);
        cp2 += strlen(cp2) + 1;
    }

    /*
     * Set up the page tables for the process so that we can read the
     * program into memory.    Get the right number of physical pages
     * allocated, and set them all to writable.
     */

// ==>> Throw away the old region 1 virtual address space of the
// ==>> curent process by freeing
// ==>> all physical pages currently mapped to region 1, and setting all 
// ==>> region 1 PTEs to invalid.
// ==>> Since the currently active address space will be overwritten
// ==>> by the new program, it is just as easy to free all the physical
// ==>> pages currently mapped to region 1 and allocate afresh all the
// ==>> pages the new program needs, than to keep track of
// ==>> how many pages the new process needs and allocate or
// ==>> deallocate a few pages to fit the size of memory to the requirements
// ==>> of the new process.

    int rc;
    // Clean slate
    rc = unmap_page_to_frame(user_page_table, 0, GET_PAGE_NUMBER(VMEM_1_SIZE));
    if(rc) {
        log_info("Clean slate err");
        return _FAILURE;
    }
    log_info("Clean page table DONE");
    set_user_page_table(proc->page_table);
    log_info("Set page table done.");

    /* Set user memory boundaries*/
    user_memory.text_low     = (unsigned int)VMEM_1_BASE;
    user_memory.text_high    = (unsigned int)(text_pg1 + li.t_npg + VMEM_1_BASE);
    user_memory.data_low     = (unsigned int)(data_pg1 + VMEM_1_BASE);
    user_memory.data_high    = (unsigned int)(data_pg1 + data_npg + VMEM_1_BASE);
    user_memory.brk_low      = user_memory.data_high;
    user_memory.brk_high     = user_memory.data_high;
    user_memory.stack_low    = (unsigned int)cpp;
    user_memory.stack_high   = (unsigned int)(cpp + stack_npg); 

    log_info("text_pg1 => %p, text_npg => %d", text_pg1 + VMEM_1_BASE, li.t_npg);
    log_info("data_pg1 => %p, data_npg => %d", data_pg1 + VMEM_1_BASE, data_npg);
    log_info("stack_pg1 => %p, stack_npg => %d", cpp, stack_npg);

// ==>> Allocate "li.t_npg" physical pages and map them starting at
// ==>> the "text_pg1" page in region 1 address space.    
// ==>> These pages should be marked valid, with a protection of 
// ==>> (PROT_READ | PROT_WRITE).
    rc = map_page_to_frame(user_page_table, text_pg1, text_pg1 + li.t_npg, PROT_WRITE | PROT_READ);
    if(rc) {
        log_err("Map text seg error");
        return _FAILURE;
    }
    log_info("Map user text DONE");
    
// ==>> Allocate "data_npg" physical pages and map them starting at
// ==>> the    "data_pg1" in region 1 address space.    
// ==>> These pages should be marked valid, with a protection of 
// ==>> (PROT_READ | PROT_WRITE).
    rc = map_page_to_frame(user_page_table, data_pg1, data_pg1 + data_npg, PROT_WRITE | PROT_READ);
    if(rc) {
        log_err("map data seg error");
        return _FAILURE;
    }
    log_info("Map user data DONE");

    /*
     * Allocate memory for the user stack too.
     */
// ==>> Allocate "stack_npg" physical pages and map them to the top
// ==>> of the region 1 virtual address space.
// ==>> These pages should be marked valid, with a
// ==>> protection of (PROT_READ | PROT_WRITE).
    rc = map_page_to_frame(user_page_table, 
                            GET_PAGE_NUMBER(VMEM_1_SIZE) - stack_npg, 
                            GET_PAGE_NUMBER(VMEM_1_SIZE), 
                            PROT_READ | PROT_WRITE);
    if(rc) {
        log_err("Map stack seg error");
        return _FAILURE;
    }
    log_info("Map user stack with #%d pages DONE", stack_npg);

    /*
     * All pages for the new address space are now in the page table.    
     * But they are not yet in the TLB, remember!
     */
    /*
     * Read the text from the file into memory.
     */
    lseek(fd, li.t_faddr, SEEK_SET);
    segment_size = li.t_npg << PAGESHIFT;
    if (read(fd, (void *) li.t_vaddr, segment_size) != segment_size) {
        close(fd);
// ==>> KILL is not defined anywhere: it is an error code distinct
// ==>> from ERROR because it requires different action in the caller.
// ==>> Since this error code is internal to your kernel, you get to define it.
        return KILL;
    }
    /*
     * Read the data from the file into memory.
     */
    lseek(fd, li.id_faddr, 0);
    segment_size = li.id_npg << PAGESHIFT;

    if (read(fd, (void *) li.id_vaddr, segment_size) != segment_size) {
        close(fd);
        return KILL;
    }

    log_info("Read file DONE");

    /*
     * Now set the page table entries for the program text to be readable
==>> that these pages will have indices starting at text_pg1 in 
==>> the page table for region 1.
==>> The new protection should be (PROT_READ | PROT_EXEC).
==>> If any of these page table entries is also in the TLB, either
==>> invalidate their entries in the TLB or write the updated entries
==>> into the TLB.  It's nice for the TLB and the page tables to remain
==>> consistent.*/

    close(fd);			/* we've read it all now */
    set_ptes(user_page_table, text_pg1, text_pg1 + li.t_npg, PROT_READ | PROT_EXEC); 
    log_info("Set ptes DONE");

  /*
   * Zero out the uninitialized data area
   */
    log_info("About to bzero from %p to %p", (void*)li.id_end, (void*)(li.id_end + li.ud_end - li.id_end ));
    bzero((void*)li.id_end, li.ud_end - li.id_end);
    log_info("Bzero DONE");

  /*
   * Set the entry point in the exception frame.
   */
// ==>> Here you should put your data structure (PCB or process)
// ==>>  proc->context.pc = (caddr_t) li.entry;
    proc->user_context.pc = (caddr_t) li.entry;

  /*
   * Now, finally, build the argument list on the new stack.
   */

#ifdef LINUX
    memset(cpp, 0x00, VMEM_1_LIMIT - ((int) cpp));
#endif

    *cpp++ = (char *)argcount;		/* the first value at cpp is argc */
    cp2 = argbuf;
    for (i = 0; i < argcount; i++) {      /* copy each argument and set argv */
        *cpp++ = cp;
        strcpy(cp, cp2);
        cp += strlen(cp) + 1;
        cp2 += strlen(cp2) + 1;
    }
    free(argbuf);
    *cpp++ = NULL;			/* the last argv is a NULL pointer */
    *cpp++ = NULL;			/* a NULL pointer for an empty envp */

    return _SUCCESS;
}


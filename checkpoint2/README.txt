/src
kernel_init.c:  used to set kernel data, init the vector, page table, registers, and enable the VM,  to start the kernel
memory.c: used to init frams and  map pages to frames
list.c: used to define the list data structure which used in memory.c
traps.c: write the skeletal code of the trap_memory_handler function

Future work:
dlist.c
pipe.c
proc.c
sys.c
timer.c
tty.c
malloc.c
lock.c
load_program.c

/include
list.h: define some data used to implement the list data structure
memory.h: define vm data structure and some other related data structures
traps.h: define the trap handler functions
common.h: define the common use macros

reference:
debug.h: Code reference from: Zed's Awesome Debug Macros (url: http://c.learncodethehardway.org/book/ex20.html

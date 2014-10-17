/src
kernel_init.c: Initiate trap vectors and memory management (including page table building and frames mapping), implement SetKernelBrk
memory.c: Memory management for paging, mapping
traps.c: Write the skeletal code of the trap_memory_handler function
list.c: used to define the list data structure which used in memory.c and some future purpose
bitmap.c: used for memory.c, going to replace list.c at next checkpoint

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

reference:
debug.h: Code reference from: Zed's Awesome Debug Macros (url: http://c.learncodethehardway.org/book/ex20.html

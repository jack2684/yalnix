Team StdErr Junjie Guan, Ziyang Wang

In checkpoint 3, we have completed:
I. 	    Implemented getpid(), delay(), brk() syscalls.
II. 	Write a simple init process to do the test.
III.	Implemented the loadprogram function to load a program and create a process for it.
IV.	    Implemented a Round Robin Scheduler to switch between the init and idle process with a fixed time ticks.(Say the default ticks is set to DEFAULT_QUANTUMN.)
V. 	    Implemented the process queue to arrange the waiting and ready processes, and delay queue for Delay() processes
VI.     Implemented the doubly linked list to achieve O(1) node removal

To test checkpoint 3:
I. 	    Enter "make" command in the ./yalnix
II. 	Enter "./yalnix -n -t -lk 5 -lu 5 src/init" command to run this program.

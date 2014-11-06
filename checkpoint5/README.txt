Team StdErr Junjie Guan, Ziyang Wang

In checkpoint 5, we have completed:
I. 		Implement TtyWrite() and TtyRead() syscalls.
II. 	Implement all the remaing traps, including TRAP_TTY_TRANSMIT and TRAP_TTY_RECEIVE.
III.    Start to implement pipe related syscalls.
IV.     Implement a simple hashmap for id=>pointer look up.
V.      Implement an ID manager as helper to generate and recollect IDs for pcb, pipe, locks, etc.
VI.  	Complete the Priority 3 for checkpoint 5 on the canvas.


To test checkpoint 4:
I. 		Enter "make" command in the ./yalnix.
II. 	Enter "./yalnix -t -lk 5 -lu 5” command to run this program

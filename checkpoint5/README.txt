Team StdErr Junjie Guan, Ziyang Wang

In checkpoint 5, we have completed:
I. 		Implement TtyWrite() and TtyRead() syscalls.
            *Noted that I set the TERMINAL_MAX_LINE to be very small, which is 10. Just to test
II. 	Implement all the remaing traps, including TRAP_TTY_TRANSMIT and TRAP_TTY_RECEIVE.
III.    Start to implement pipe related syscalls.
IV.     Implement a simple hashmap for id=>pointer look up.
V.      Implement an ID manager as helper to generate and recollect IDs for pcb, pipe, locks, etc.
VI.     Implement pipe and test. 
            *This design is not yet perfect. Currently I do not accept read/write larger than pipe size. Will improve it later
VII.  	Complete the Priority 3 for checkpoint 5 on the canvas.


To test checkpoint 4:
I. 		Enter "make" command in the ./yalnix.
II. 	Enter "./yalnix -t -lk 5 -lu 5” command to run this program
            i)      in terminal 1 you can test tty read and write
            ii)     in terminal 2 you can test and try pipe


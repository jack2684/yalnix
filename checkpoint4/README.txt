Team StdErr Junjie Guan, Ziyang Wang

In checkpoint 4, we have completed:
I. 	Implemente fork(), exec(), wait(), exit() syscalls.
II. 	Improve physical memory copy and init kernel context function which are left from checkpoint3.
III.	Improve the context switch function's performance.
IV. 	Build some test cases for the new sys-calls.
V. 	Implement memory trap to handle growing user stack. 
VI.	Completed the Priority 5 in Checkpoint 4 on Canvas.


To test checkpoint 4:
I. 	Enter "make" command in the ./yalnix.
II. 	Enter "./yalnix -n -t -lk 5 -lu 5” or "./yalnix -n -t -lk 5 -lu 5 <delay>“ command to run this program
		by default, delay is 1. You can remove lk for clearer TRACE

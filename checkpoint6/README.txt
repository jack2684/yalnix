Team StdErr Junjie Guan, Ziyang Wang

In checkpoint 6, we have completed:
I. 	Implement Lock and Cvar related syscalls.
II.     Implement a simple inter-process synchronization, along with custom0 syscall, for cvar testing
III.	Improve the pipe syscall performance and robusty.
IV 	Complete the highest Priority for checkpoint 6 on the canvas.


To test checkpoint 4:
I. 	Enter "make" command in the ./yalnix.
II. 	Enter "./yalnix -t -lk 5 -lu 5” command to run this program
		i)	in terminal 1 you can test cvar syscall
		ii)	in terminal 2 you can test pipe syscall
        	iii)	in terminal 3 you can test lock syscall

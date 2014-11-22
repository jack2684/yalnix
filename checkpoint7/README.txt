Team StdErr Junjie Guan, Ziyang Wang

In checkpoint 7, we have completed:
I.      Test through four test case (E.g, bigstack.c; torture.c: could run for hours).
II.     Implement grad-level feature: semaphore.
III.    Implement grad-level feature: localfork, for thread-like suppport, including feature like
            a. parent and child-threads share data and text segment
            b. when child exit, it cannot free data and text segment
            c. when parent exit, all the child should release resources and exit
IV.     Implement a process torture case (test.proc), to test complex process garbage collection of resrouce like pipe, lock, cvar and process itself 
IV.     Successfully transform the leynard bridge program into yalnix.
V.      Complete the Yalnix project.

To test checkpoint 7:
I.      Enter "make" command in the ./yalnix.
II.     Enter "./yalnix -t -lk 5 -lu 5 src/test.****” command to run this program
III.    For bridge test, here are some recommended input:
    1: ./yalnix -t -lk 5 -lu 5 src/test.bridge 6 15 15 20
    2: ./yalnix -t -lk 5 -lu 5 src/test.bridge 6 5 20 20
    3: ./yalnix -t -lk 5 -lu 5 src/test.bridge 6 10 25 20
    *noted that, for the third case, two many thread would empty the thread pool 

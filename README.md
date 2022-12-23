# Assignment 5: OSN #
## Question 1

- compiled as:
>  gcc q1.c -o q1 -pthread -lrt

- executable file:
> ./q1

in this question, the students act as threads and the machines act as semaphores.

the threads are executed on a first come first serve basis, according to their arrival times

the taken input is sorted based on arrival time, using qsort and a custom sorting function

each thread is slept until its arrival time occurs, in this time other students can also look check for available machines.

once student's arrival time is there, we execute the remaining routine function.

in this function, we wait until we have access to a machine, incase it is not used by anyone else.

if we have to wait for more than patience time, we say student couldnt wash, increment the timewasted time and number of non washed student's count and stop the routine

but if the machine is available, we let the student wash by sleeping for it's washing time number of seconds, increment timewasted [if any], and print when it leaves washing

all four threads get executed simultaneously and we use semaphores to lock the machines

the machine is empty if semaphore is not locked i.e semaphore value >= 0
the machine cannot be used by one more student if the value is currently 0, once machine is free, the semaphore value is incremented by sem_post() function, and the lock is hence removed. other threads can now execute this critical section [machines].
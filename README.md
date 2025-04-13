# OS simulator ;)

## general specification

the project aims to be an OS core simulator, with a graphic interface that allows choices (system control) from who executes the simulation.

there are two functions that returns int values, that correspond to the id code of each thread implemented:

    interruptControl: triggered when a external event occurs.
    sysCall: triggered when a process's event occurs.

from the integer obtained from one of the functions, the core activate the responsible thread (with the scheduling of the next process).
the possible origins of events are:

    
    - processInterrupt (triggered by processCreate() and the end of I/O operation)
    - semaphoreP (treatment of process locking)
    - semaphoreV (treatment of process unlocking)
    - DiskRequest (I/O request)
    - DiskFinish (sinalization of I/O end)
    - PrintRequest (printing call)
    - PrintFinish (end of printing)
    - memLoadReq (loading memory request)
    - memLoadFinish (end of load from memory)t
    - fsRequest (file system request)
    - fsFinish (end of fs request)
    - processCreate (create a process on BCP)
    - processFinish (pop a process of BCP)

## system functionality

the system treatment must be made with a virtual clock.
the functions interruptControl and sysCall must act upon synthetics program commands.

## synthetic program definition

a synthetic program is defined by a file (read by OS) that has two parts:

    HEADER
        - program name (one word)
        - segment id (a integer)
        - original priority of program (a integer)
        - segment length (in kbytes)
        - list of semaphores used by program (splited by spaces)

    COMMANDS SEQUENCE
        - exec t (indicates that the program will be executed for t times units)
        - read k (indicates that the program will read the disk on trail k)
        - write k (will write on trail k)
        - P(s) (indicates that the program will access the critical section s)
        - V(s) (indicates that the program will free the critical section s)
        - print t (will print the results by t time units)

example: https://www.dcce.ibilce.unesp.br/~aleardo/cursos/sisop/synthetic_2

## obs

to better undestanding to how program using threads see this doc: https://www.dcce.ibilce.unesp.br/~aleardo/cursos/hpc/unixpar2024.pdf

use "semaphore.h" and "pthreads.h" of Linux



# OS simulator ;)

## general specification

the project aims to be an OS core simulator, with a graphic interface that allows choices (system control) from who executes the simulation.

we must implement the core from the implementation of the following threads. the call for one of them are going to be managed by two functions that returns integer values, that correspond to the id code of each thread implemented:

    interruptControl: triggered when a external event occurs.
    sysCall: triggered when a process's event occurs.

from the integer obtained from one of the functions, the core activate the relative thread (with the scheduling of the next process).
the possible origins of the events are (the n° identify each event):

    
    - processInterrupt (1) -> triggered by processCreate() and the end of I/O operation
    - semaphoreP (10) -> treatment of process locking
    - semaphoreV (11) -> treatment of process unlocking
    - DiskRequest (4) -> I/O request
    - DiskFinish (5) -> sinalization of I/O end
    - PrintRequest (14) -> printing call
    - PrintFinish (15) -> end of printing
    - memLoadReq (6) -> loading memory request
    - memLoadFinish (7) -> end of load from memory
    - fsRequest (8) -> file system request
    - fsFinish (9) -> end of fs request
    - processCreate (2) -> create a process on BCP
    - processFinish (3) -> pop a process of BCP

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



delivery date: 05/05

## description

the first part must has the interface of system control and the following functions:

    1. processInterrupt (triggered by processCreate() and the end of I/O operation)
    2. semaphoreP (treatment of process locking)
    3. semaphoreV (treatment of process unlocking)
    4. memLoadReq (loading memory request)
    5. memLoadFinish (end of load from memory)
    6. processCreate (create a process on BCP)
    7. processFinish (pop a process of BCP)

## schedular algorithm

the process scheduling must uses an algorithm based on frequency of I/O requests. at this moment, just consider that the schedular will choose the process with bigger read and write requests already made by the process (in a draw case, chooses by ID process on BCP)

## memory management

the memory uses a paged schema, defining the resident conjunt of each process with a 16 pages length.
for the process's pages splitting, consider that for "exec" instructions it's counted 1 page for each 1000 time units of execution.
consider the memory total length as 1 Mbytes, being 64 kbytes used by OS (including table pages) and each page with 1kbytes.
for the pagning mechanism it's used the second chance algorithm.

## control interface

the control interface must allows that new process be submited, visualizing data about the state of running process and about memory ocupation.

obs.: the info view must be explicited required, for default just the info about the running process will be showed.



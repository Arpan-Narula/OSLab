Question 1
Added system calls int mprotect(void *addr, int len) and int munprotect(void *addr, int len) across files syscall.h, user.h, usys.S, syscall.c, defs.h, sysproc.c, vm.c
mprotect clears PTE_W, munprotect sets PTE_W
Wrote user program prottest.c which gets a page by sbrk and then tests the implemented mprotect and munprotect functions by writing in different states of the page (writeable and read only)

Question 2
Added int priority field in struct proc in proc.h
Set default value to 10 in allocproc in proc.c
Implemented system call int set_priority(int pid, int priority) across files syscall.h, user.h, usys.S, syscall.c, defs.h, sysproc.c, proc.c
proc.c contains the main logic of the system call
Modified scheduler in proc.c to select the process with the lowest priority
Created user program schedtest.c where parent forks 3 childs with different priorities and gives them long task, the scheduler selects the one with lower priority first.
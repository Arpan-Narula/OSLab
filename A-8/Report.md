# Assignment 8 - Memory Management in xv6
## Report (Q1 to Q4)

### Overview

For this assignment I worked on memory management part of xv6, did 4
questions - va2pa syscall, getmeminfo syscall, vmprint syscall, and lazy
allocation for heap. For first 3 I added new syscalls using the usual xv6
method (syscall.h, syscall.c, usys.S, user.h and a handler function). For Q4
I modified the existing sbrk and trap handling instead of adding new
syscall.

### Question 1 - va2pa

I used the walkpgdir() function already in vm.c to walk the page table and
get the PTE for a virtual address. walkpgdir was static though, so i had to
remove static and add its prototype in defs.h so other files could use it.
Function returns -1 if page not present, or if PTE_U bit not set (kernel
only page). For the physical address, i took frame no. from the PTE and OR'd
it with lower 12 bits (offset) of the virtual address.

**Problem faced:** got "unknown type name pte_t" error in defs.h, since
pte_t is defined in mmu.h and defs.h wasnt including it. fixed by adding
#include "mmu.h" near top of defs.h.

**Result:** global, local and heap(malloc) variables all gave different
valid physical addresses, invalid addr 0x7FFFFFFF gave -1 as expected. after
fork, parent and child showed same virtual address for global_var but
different physical address - proves xv6 copies the page on fork rather than
doing copy-on-write.

### Question 2 - getmeminfo

Added a free_pages counter in the kmem struct in kalloc.c, incremented in
kfree() and decremented in kalloc(), both under kmem.lock. Made a new struct
meminfo in types.h (so kernel and user code both can use it) with fields
total_pages, free_pages, proc_pages, pagetable_pages. total_pages is just
PHYSTOP/PGSIZE. proc_pages is sz/PGSIZE. pagetable_pages i counted as 1 (page
directory) plus one for every valid PDE.

**Testing:** ran memtest.c, free pages went down after malloc, went down
more after fork (childs copy), came back up after child exits and parent
waits, and back to original after free(). so no memory leak happening.

### Question 3 - vmprint

vmprint() walks all 1024 entries of page directory, for ones thats present
and below KERNBASE i print index, phys addr of page table and P/W/U flags,
then loop through that page tables 1024 entries same way but indented, to
show 2 level structure. entries at/above KERNBASE just get counted and
printed as one line total at the end.

Guard page: exec() maps a page below the stack with PTE_P set but PTE_U not
set, so in output it shows as U=0 while pages around it show U=1, thats how
i identified it. checked output before/after sbrk(8*4096), saw 8 new entries
appear, and after sbrk(-4096) the top entry disappeared. also verified no.
of user pages (U=1) matched sz/PGSIZE minus 1 for guard page.

### Question 4 - Lazy Allocation

This one was hardest. changed sys_sbrk() so when n>0 it just does
myproc()->sz += n, doesnt call growproc() to actually allocate. n<0 still
calls growproc() normally so shrinking frees real pages.

Added T_PGFLT case in trap.c - read fault addr with rcr2(), round down to
page boundary. if addr >= sz or >= KERNBASE, its a real segfault, kill
process. if a PTE already exists for that addr (like guard page) thats a
protection fault, also kill. otherwise allocate with kalloc(), zero it, map
with mappages() using PTE_W|PTE_U.

**Problems faced:**

1. "implicit declaration of mappages" in trap.c - mappages had no prototype
in defs.h, same issue as walkpgdir in Q1, added prototype to fix.

2. then got "static declaration of mappages follows non static declaration"
- turns out mappages() was also static in vm.c, had to remove static there
too.

3. had to fix copyuvm() in vm.c - it does panic() if PTE not present while
forking, but with lazy alloc thats normal (page never touched). changed it
to just continue to next page instead of panic.

4. checked deallocuvm() (used on exit/shrink), it already handles missing
PTEs fine without panic, so no change needed there.

**Testing:** lazytest.c does sbrk(32MB), free_pages barely changes right
after since nothing actually allocated yet. after touching a few pages,
free_pages drops by exactly that many. after fork it drops more (touched
pages getting copied to child), goes back up after child exits. also tried
reading a file into an untouched buffer and it worked fine, confirms kernel
side access to lazy pages works too since kernel uses same page table as the
process.

### Summary

Q1 added va2pa syscall - files changed: vm.c, defs.h, sysproc.c, syscall.h,
syscall.c, user.h, usys.S, Makefile.
Q2 added getmeminfo syscall - same files as above plus types.h and kalloc.c.
Q3 added vmprint syscall - same files as Q1.
Q4 didnt add new syscall, modified sysproc.c, trap.c, vm.c and defs.h.

Overall everything works together, i re-ran vatest, memtest and vmprinttest
after doing Q4 changes to make sure lazy allocation didnt break anything
from before, and they still gave correct output. free_pages count always
comes back to original baseline after fork+exit cycles so no leaks.
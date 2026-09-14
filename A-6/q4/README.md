Model: 5 real xv6 processes; 3 resource types with limited instances:
printer=2, scanner=1, disk=2. Each process needs a fixed pair (2 out of 3).
Each pool is a counting kernel semaphore initialized to its instance count,
so sem\_wait/sem\_signal take/return one instance - available instances can
never be exceeded (semaphore invariant), and a mutex semaphore protects a
shared-memory mirror (avail\[3]) used to print live availability.

STRATEGY: resource ordering (hierarchical allocation, same as Q3). Every
process requests its two resources in ascending global type order
(printer < scanner < disk). Justification: circular wait requires a cycle in
the wait-for graph; with a global order, a blocked process only ever waits
for processes holding HIGHER-ranked types, so ranks strictly increase along
any wait chain and a cycle is impossible. Combined with bounded hold times
(sleep(8)) and full release every cycle, deadlock cannot occur.
(Alternative considered: Banker-style admission check from Q1 before each
grant - correct but heavier; ordering needs no global state inspection.)

Output shows, per process and cycle: request -> granted (with live avail) ->
WORKING -> releasing (with live avail). All 5 processes complete 4 cycles and
the parent confirms termination; availability never goes negative or above
the instance limits at any point.



Build/run: make clean \&\& make qemu-nox ; $ syncdeadlock


Setup: two shared locks = binary kernel semaphores (custom sem\_init/sem\_wait/
sem\_signal syscalls from the earlier sync assignment, sleep/wakeup based).
resourceorder.c forks two real processes A and B with a deliberate busy-wait
delay between the two lock acquisitions to make the clash reproducible.

BAD run: A takes Lock1 then Lock2; B takes Lock2 then Lock1. Each holds what
the other needs -> circular wait -> both block forever (log shows the four
step markers and then silence; a parent watchdog reports the hang after 50
ticks as the timeout/interrupt note).



FIXED run: both processes acquire in the SAME global order (Lock1 then
Lock2). The same workload completes; parent confirms both COMPLETED.

Why consistent ordering eliminates circular wait: circular wait needs a cycle
in the wait-for graph. With a global lock order, a process waiting for a lock
only ever waits for processes holding HIGHER-ordered locks (it must acquire
lower-ordered ones first). Along any wait chain lock ranks strictly increase,
so a cycle - which would need ranks to increase and return to the start - is
impossible. Hence the circular-wait condition (one of the four necessary
conditions for deadlock) can never hold.

Build/run: make clean \&\& make qemu-nox ; $ resourceorder fixed ; $ resourceorder bad


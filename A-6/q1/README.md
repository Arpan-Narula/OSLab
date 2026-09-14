Design: bankers.c models n=5 processes, m=3 resource types with
Allocation\[5]\[3], Max\[5]\[3], Available\[3]; Need\[i]\[j] = Max - Allocation
is computed at runtime.
Safety algorithm: work-vector + finish flags; repeatedly "finish" any process
with Need <= work (work += its Allocation); if all finish, the state is safe
and the finish order is printed as the safe sequence; otherwise unsafe.
Resource-request algorithm: reject if Request > Need or Request > Available;
otherwise pretend-grant, re-run safety; commit if still safe, else roll back
and print "Request denied - would lead to unsafe state".
Test scenarios (built-in): P1 (1,0,2) GRANTED; P4 (3,3,0) DENIED (exceeds
Available); P0 (0,2,0) DENIED (unsafe after simulation, rolled back).
CLI mode also supported via xv6 argv: bankers <pid> <r0> <r1> <r2>.
Build/run: make clean \&\& make qemu-nox ; at $ prompt: bankers


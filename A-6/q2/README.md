Model: single-instance resources, so the RAG reduces to a wait-for graph.
deadlockdetect.c stores an allocation matrix (who holds each resource) and a
request matrix (who waits for each resource). Wait-for graph construction:
edge P\_i -> P\_j exists iff P\_i requests some resource r that P\_j holds.
Cycle detection: DFS with white/gray/black coloring; a back-edge to a GRAY
node (currently on the recursion stack) means a cycle; the recursion stack
(path\[]) from that node onward is printed as the exact deadlock cycle.
Scenarios: (1) acyclic chain -> NO DEADLOCK; (2) 3-process circular wait
P0->P1->P2->P0 -> DEADLOCK + printed cycle; (3) 2-process cycle P0<->P1.
The log shows matrices, every wait-for edge with its reason, the detection
result, and the exact cycle when one exists.


Build/run: make clean \&\& make qemu-nox ; $ deadlockdetect


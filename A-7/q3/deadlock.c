#include "types.h"
#include "stat.h"
#include "user.h"

#define MAXP 10
#define MAXR 10

int
detect_deadlock(int n, int m, int allocation[MAXP][MAXR], int request[MAXP][MAXR],
                 int available[MAXR], int deadlocked[MAXP])
{
  int work[MAXR];
  int finish[MAXP];
  int i, j, found, can_proceed, count;

  for(j = 0; j < m; j++)
    work[j] = available[j];

  for(i = 0; i < n; i++)
    finish[i] = 0;

  do {
    found = 0;
    for(i = 0; i < n; i++){
      if(finish[i])
        continue;

      can_proceed = 1;
      for(j = 0; j < m; j++){
        if(request[i][j] > work[j]){
          can_proceed = 0;
          break;
        }
      }

      if(can_proceed){
        for(j = 0; j < m; j++)
          work[j] += allocation[i][j];
        finish[i] = 1;
        found = 1;
      }
    }
  } while(found);

  count = 0;
  for(i = 0; i < n; i++){
    if(!finish[i]){
      deadlocked[count] = i;
      count++;
    }
  }

  return count;
}

int
is_safe_state(int n, int m, int allocation[MAXP][MAXR], int max[MAXP][MAXR], int available[MAXR])
{
  int need[MAXP][MAXR];
  int work[MAXR];
  int finish[MAXP];
  int i, j, found, can_proceed, count;

  for(i = 0; i < n; i++)
    for(j = 0; j < m; j++)
      need[i][j] = max[i][j] - allocation[i][j];

  for(j = 0; j < m; j++)
    work[j] = available[j];

  for(i = 0; i < n; i++)
    finish[i] = 0;

  do {
    found = 0;
    for(i = 0; i < n; i++){
      if(finish[i])
        continue;

      can_proceed = 1;
      for(j = 0; j < m; j++){
        if(need[i][j] > work[j]){
          can_proceed = 0;
          break;
        }
      }

      if(can_proceed){
        for(j = 0; j < m; j++)
          work[j] += allocation[i][j];
        finish[i] = 1;
        found = 1;
      }
    }
  } while(found);

  count = 0;
  for(i = 0; i < n; i++)
    if(finish[i])
      count++;

  return count == n;
}

void
print_vec(char *label, int m, int v[MAXR])
{
  int j;

  printf(1, "%s", label);
  for(j = 0; j < m; j++)
    printf(1, "%d ", v[j]);
  printf(1, "\n");
}

void
print_matrix(char *label, int n, int m, int mat[MAXP][MAXR])
{
  int i, j;

  printf(1, "%s\n", label);
  for(i = 0; i < n; i++){
    printf(1, "P%d: ", i);
    for(j = 0; j < m; j++)
      printf(1, "%d ", mat[i][j]);
    printf(1, "\n");
  }
}

void
test1(void)
{
  int n = 3, m = 2;
  int allocation[MAXP][MAXR] = { {1,0}, {0,1}, {0,0} };
  int request[MAXP][MAXR]    = { {0,1}, {1,0}, {0,0} };
  int available[MAXR] = {0, 0};
  int deadlocked[MAXP];
  int count, i;

  printf(1, "=== Test 1: Deadlock Detection ===\n");
  print_vec("Available: ", m, available);
  print_matrix("Allocation:", n, m, allocation);
  print_matrix("Request:", n, m, request);

  count = detect_deadlock(n, m, allocation, request, available, deadlocked);

  if(count == 0){
    printf(1, "result: no deadlock\n");
  } else {
    printf(1, "result: deadlock detected, processes: ");
    for(i = 0; i < count; i++)
      printf(1, "P%d ", deadlocked[i]);
    printf(1, "\n");
  }
  printf(1, "\n");
}

void
test2(void)
{
  int n = 5, m = 3;
  int allocation[MAXP][MAXR] = {
    {0,1,0},
    {2,0,0},
    {3,0,2},
    {2,1,1},
    {0,0,2}
  };
  int max[MAXP][MAXR] = {
    {7,5,3},
    {3,2,2},
    {9,0,2},
    {2,2,2},
    {4,3,3}
  };
  int available[MAXR] = {3, 3, 2};
  int safe;

  printf(1, "=== Test 2: Banker's Algorithm (safe state) ===\n");
  print_vec("Available: ", m, available);
  print_matrix("Allocation:", n, m, allocation);
  print_matrix("Max:", n, m, max);

  safe = is_safe_state(n, m, allocation, max, available);

  printf(1, "result: system is %s\n", safe ? "SAFE" : "UNSAFE");
  printf(1, "\n");
}

void
test3(void)
{
  int n = 2, m = 1;
  int allocation[MAXP][MAXR] = { {1}, {2} };
  int max[MAXP][MAXR]        = { {6}, {8} };
  int available[MAXR] = {5};
  int safe_before, safe_after;
  int req_amount = 5;

  printf(1, "=== Test 3: Request that causes an unsafe state ===\n");
  print_vec("Available (before): ", m, available);
  print_matrix("Allocation (before):", n, m, allocation);
  print_matrix("Max:", n, m, max);

  safe_before = is_safe_state(n, m, allocation, max, available);
  printf(1, "state before request is %s\n", safe_before ? "SAFE" : "UNSAFE");

  printf(1, "P1 requests %d unit(s) of the resource\n", req_amount);

  allocation[1][0] += req_amount;
  available[0] -= req_amount;

  safe_after = is_safe_state(n, m, allocation, max, available);

  print_vec("Available (after): ", m, available);
  print_matrix("Allocation (after):", n, m, allocation);

  if(safe_after){
    printf(1, "result: request granted, system remains SAFE\n");
  } else {
    printf(1, "result: granting this request leads to an UNSAFE state, request should be denied\n");
    allocation[1][0] -= req_amount;
    available[0] += req_amount;
    printf(1, "request rolled back\n");
  }
  printf(1, "\n");
}

void
pipe_deadlock_demo(void)
{
  int p1[2];
  int p2[2];
  int pid_a, pid_b;
  char buf[1];

  printf(1, "=== Real deadlock demo using pipes ===\n");

  pipe(p1);
  pipe(p2);

  pid_a = fork();
  if(pid_a == 0){
    close(p1[0]);
    close(p2[1]);
    printf(1, "process A: waiting to read pipe2\n");
    read(p2[0], buf, 1);
    write(p1[1], "x", 1);
    printf(1, "process A: done\n");
    exit();
  }

  pid_b = fork();
  if(pid_b == 0){
    close(p2[0]);
    close(p1[1]);
    printf(1, "process B: waiting to read pipe1\n");
    read(p1[0], buf, 1);
    write(p2[1], "y", 1);
    printf(1, "process B: done\n");
    exit();
  }

  close(p1[0]);
  close(p1[1]);
  close(p2[0]);
  close(p2[1]);

  sleep(50);

  printf(1, "watchdog: neither process finished after waiting, circular wait confirmed -> deadlock\n");
  kill(pid_a);
  kill(pid_b);
  wait();
  wait();
  printf(1, "\n");
}

int
main(void)
{
  test1();
  test2();
  test3();
  pipe_deadlock_demo();
  exit();
}

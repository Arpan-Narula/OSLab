#include "types.h"
#include "stat.h"
#include "user.h"

#define NTHREADS 3
#define NITER 100000
#define SEM_MUTEX 0

volatile int counter = 0;

void
increment(void *arg)
{
  int i;

  for(i = 0; i < NITER; i++){
    sem_wait(SEM_MUTEX);
    counter = counter + 1;
    sem_signal(SEM_MUTEX);
  }

  exit();
}

int
main(void)
{
  int i;
  int pids[NTHREADS];
  void *stacks[NTHREADS];

  sem_init(SEM_MUTEX, 1);

  for(i = 0; i < NTHREADS; i++){
    stacks[i] = malloc(4096);
    pids[i] = clone(increment, 0, stacks[i]);
    if(pids[i] < 0){
      printf(1, "clone failed\n");
      exit();
    }
  }

  for(i = 0; i < NTHREADS; i++){
    join();
  }

  printf(1, "final counter value: %d\n", counter);
  exit();
}

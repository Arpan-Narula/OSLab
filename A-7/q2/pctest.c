#include "types.h"
#include "stat.h"
#include "user.h"

#define BUFSIZE 5
#define NITEMS 10

#define SEM_EMPTY 0
#define SEM_FULL 1
#define SEM_MUTEX 2

int buffer[BUFSIZE];
int in = 0;
int out = 0;

void
producer(void *arg)
{
  int i;
  int item;

  for(i = 0; i < NITEMS; i++){
    item = i;

    sem_wait(SEM_EMPTY);
    sem_wait(SEM_MUTEX);

    buffer[in] = item;
    printf(1, "produced %d at index %d\n", item, in);
    in = (in + 1) % BUFSIZE;

    sem_signal(SEM_MUTEX);
    sem_signal(SEM_FULL);
  }

  exit();
}

void
consumer(void *arg)
{
  int i;
  int item;

  for(i = 0; i < NITEMS; i++){
    sem_wait(SEM_FULL);
    sem_wait(SEM_MUTEX);

    item = buffer[out];
    printf(1, "consumed %d from index %d\n", item, out);
    out = (out + 1) % BUFSIZE;

    sem_signal(SEM_MUTEX);
    sem_signal(SEM_EMPTY);
  }

  exit();
}

int
main(void)
{
  void *stack1;
  void *stack2;
  int pid1;
  int pid2;

  sem_init(SEM_EMPTY, BUFSIZE);
  sem_init(SEM_FULL, 0);
  sem_init(SEM_MUTEX, 1);

  stack1 = malloc(4096);
  stack2 = malloc(4096);

  pid1 = clone(producer, 0, stack1);
  pid2 = clone(consumer, 0, stack2);

  if(pid1 < 0 || pid2 < 0){
    printf(1, "clone failed\n");
    exit();
  }

  join();
  join();

  printf(1, "producer-consumer finished\n");
  exit();
}

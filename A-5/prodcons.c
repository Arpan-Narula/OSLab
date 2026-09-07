#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUFFER_SIZE 5
#define TOTAL_ITEMS 20
#define SEM_MUTEX 0
#define SEM_EMPTY 1
#define SEM_FULL  2

struct pc_buffer {
  int buffer[BUFFER_SIZE];
  int in, out;
};

void delay(int count) {
  for (volatile int i = 0; i < count * 100000; i++);
}

int main(void) {
  struct pc_buffer *shm = (struct pc_buffer *)shm_get(0);
  shm->in = 0; shm->out = 0;

  sem_init(SEM_MUTEX, 1);
  sem_init(SEM_EMPTY, BUFFER_SIZE);
  sem_init(SEM_FULL, 0);

  if (fork() == 0) {
    for (int i = 1; i <= TOTAL_ITEMS; i++) {
      sem_wait(SEM_FULL);
      sem_wait(SEM_MUTEX);
      printf("Consumer consumed: %d from index %d\n", shm->buffer[shm->out], shm->out);
      shm->out = (shm->out + 1) % BUFFER_SIZE;
      sem_post(SEM_MUTEX);
      sem_post(SEM_EMPTY);
      delay(3);
    }
  } else {
    for (int i = 1; i <= TOTAL_ITEMS; i++) {
      sem_wait(SEM_EMPTY);
      sem_wait(SEM_MUTEX);
      shm->buffer[shm->in] = i;
      printf("Producer produced: %d at index %d\n", i, shm->in);
      shm->in = (shm->in + 1) % BUFFER_SIZE;
      sem_post(SEM_MUTEX);
      sem_post(SEM_FULL);
      delay(1);
    }
    wait(0);
  }
  exit(0);
}

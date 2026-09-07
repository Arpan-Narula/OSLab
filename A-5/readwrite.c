#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define SEM_MUTEX 0
#define SEM_WRT   1

struct rw_data {
  int read_count;
  int shared_data;
};

void delay(int count) {
  for (volatile int i = 0; i < count * 100000; i++);
}

int main(void) {
  struct rw_data *shm = (struct rw_data *)shm_get(0);
  shm->read_count = 0; shm->shared_data = 0;
  sem_init(SEM_MUTEX, 1); sem_init(SEM_WRT, 1);

  for (int r = 0; r < 3; r++) {
    if (fork() == 0) {
      for (int i = 0; i < 4; i++) {
        sem_wait(SEM_MUTEX);
        shm->read_count++;
        if (shm->read_count == 1) sem_wait(SEM_WRT);
        sem_post(SEM_MUTEX);

        printf("Reader %d (PID %d) reading shared_data = %d (active: %d)\n", r, getpid(), shm->shared_data, shm->read_count);
        delay(2);

        sem_wait(SEM_MUTEX);
        shm->read_count--;
        if (shm->read_count == 0) sem_post(SEM_WRT);
        sem_post(SEM_MUTEX);
        delay(3);
      }
      exit(0);
    }
  }

  for (int w = 0; w < 2; w++) {
    if (fork() == 0) {
      for (int i = 0; i < 3; i++) {
        sem_wait(SEM_WRT);
        shm->shared_data++;
        printf("Writer %d (PID %d) writing shared_data = %d\n", w, getpid(), shm->shared_data);
        delay(3);
        sem_post(SEM_WRT);
        delay(4);
      }
      exit(0);
    }
  }

  for (int i = 0; i < 5; i++) wait(0);
  exit(0);
}

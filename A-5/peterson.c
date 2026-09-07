#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

struct peterson_data {
  int flag[2];
  int turn;
  int shared_counter;
};

void delay(int count) {
  for (volatile int i = 0; i < count * 100000; i++);
}

int main(void) {
  struct peterson_data *shm = (struct peterson_data *)shm_get(0);
  shm->flag[0] = 0; shm->flag[1] = 0; shm->turn = 0; shm->shared_counter = 0;

  int pid = fork();
  int id = (pid == 0) ? 1 : 0;
  int other = 1 - id;

  for (int i = 0; i < 10; i++) {
    shm->flag[id] = 1;
    shm->turn = other;
    __sync_synchronize(); 
    while (shm->flag[other] && shm->turn == other);

    shm->shared_counter++;
    printf("Process %d in CS, counter = %d\n", id, shm->shared_counter);
    delay(1);

    __sync_synchronize();
    shm->flag[id] = 0;
    delay(2);
  }

  if (pid > 0) {
    wait(0);
    printf("Final shared_counter: %d (Expected: 20)\n", shm->shared_counter);
  }
  exit(0);
}

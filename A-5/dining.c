#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NUM_PHILO 5
#define CYCLES 5

void delay(int count) {
  for (volatile int i = 0; i < count * 100000; i++);
}

int main(void) {
  for (int i = 0; i < NUM_PHILO; i++) sem_init(i, 1);

  for (int i = 0; i < NUM_PHILO; i++) {
    if (fork() == 0) {
      int left = i;
      int right = (i + 1) % NUM_PHILO;
      int first = (left < right) ? left : right;
      int second = (left < right) ? right : left;

      for (int c = 1; c <= CYCLES; c++) {
        printf("Philosopher %d: THINKING (Cycle %d)\n", i, c);
        delay(2);
        printf("Philosopher %d: HUNGRY\n", i);
        
        sem_wait(first);
        sem_wait(second);
        
        printf("Philosopher %d: EATING (Cycle %d)\n", i, c);
        delay(2);
        
        sem_post(second);
        sem_post(first);
      }
      printf("Philosopher %d: FINISHED\n", i);
      exit(0);
    }
  }

  for (int i = 0; i < NUM_PHILO; i++) wait(0);
  exit(0);
}

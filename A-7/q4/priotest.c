#include "types.h"
#include "stat.h"
#include "user.h"

#define NCHILD 4
#define WORK 50000000
#define CHECK_EVERY 2000000

int priorities[NCHILD] = {1, 5, 10, 20};

int
main(void)
{
  int i;
  int pid;
  int last_priority;
  int cur_priority;
  volatile long j;

  for(i = 0; i < NCHILD; i++){
    pid = fork();
    if(pid == 0){
      set_priority(getpid(), priorities[i]);
      last_priority = priorities[i];
      for(j = 0; j < WORK; j++){
        if(j % CHECK_EVERY == 0){
          cur_priority = get_priority(getpid());
          if(cur_priority != last_priority){
            printf(1, "pid %d priority changed %d -> %d\n", getpid(), last_priority, cur_priority);
            last_priority = cur_priority;
          }
        }
      }
      printf(1, "pid %d (start priority %d) finished\n", getpid(), priorities[i]);
      exit();
    }
  }

  for(i = 0; i < NCHILD; i++){
    wait();
  }

  printf(1, "all children finished\n");
  exit();
}

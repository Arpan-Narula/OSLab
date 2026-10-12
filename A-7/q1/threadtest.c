#include "types.h"
#include "stat.h"
#include "user.h"

volatile int counter = 0;
char stack1[4096], stack2[4096], stack3[4096];

void worker(void *arg)
{
  int i;
  for(i = 0; i < 100000; i++)
    counter++;
  exit();
}

int main(int argc, char *argv[])
{
  int pid1 = clone(worker, 0, stack1);
  int pid2 = clone(worker, 0, stack2);
  int pid3 = clone(worker, 0, stack3);

  join();
  join();
  join();

  printf(1, "final counter = %d\n", counter);
  exit();
}

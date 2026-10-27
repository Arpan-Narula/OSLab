#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  char *base;
  int i;
  int pid;

  base = sbrk(32 * 4096);

  for(i = 0; i < 5; i++){
    base[i * 4096] = 'a';
  }

  printf(1, "page faults handled: %d\n", getfaults());

  pid = fork();
  if(pid == 0){
    char *bad = base + 33 * 4096;
    *bad = 'x';
    printf(1, "child should not reach here\n");
    exit();
  } else {
    wait();
    printf(1, "parent still alive after child's bad access\n");
  }

  exit();
}

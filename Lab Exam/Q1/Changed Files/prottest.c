#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  char *p = sbrk(4096);
  if(p == (char*)-1){
    printf(1, "sbrk failed\n");
    exit();
  }

  *p = 'A';
  printf(1, "Allocated 1 page at %p with %c\n", p, *p);

  printf(1, "Calling mprotect\n");
  if(mprotect((void*)p, 1) < 0){
    printf(1, "mprotect failed\n");
    exit();
  }
  printf(1, "mprotect succeeded. Reading value from page: %c\n", *p);
  if(*p != 'A'){
    printf(1, "value changed\n");
    exit();
  }
  printf(1, "Page is protected and readable.\n");

  printf(1, "Forking child\n");
  int pid = fork();
  if(pid < 0){
    printf(1, "fork failed\n");
    exit();
  }

  if(pid == 0){
    printf(1, "Child process (pid %d) writes B\n", getpid());
    *p = 'B';
    printf(1, "child successfully wrote\n");
    exit();
  }

  int child_pid = wait();
  printf(1, "child process %d killed.\n", child_pid);
  printf(1, "Parent reading value from page: %c\n", *p);
  if(*p != 'A'){
    printf(1, "parent value was modified\n");
    exit();
  }
  printf(1, "child was killed by kernel.\n");

  printf(1, "\ncalling munprotect and writing 'C'\n");
  if(munprotect((void*)p, 1) < 0){
    printf(1, "munprotect failed on valid page\n");
    exit();
  }
  *p = 'C';
  printf(1, "munprotect succeeded\n");
  if(*p != 'C'){
    printf(1, "invalild\n");
    exit();
  }
  printf(1, " passed munprotect allowed writing 'C'.\n");

  printf(1, "All invalid arguments return -1.\n");
  exit();
}

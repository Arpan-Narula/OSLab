#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  char *p;
  int va;

  p = sbrk(4096);
  va = (int)p;

  pageinfo(va);
  setreadonly(va);
  pageinfo(va);

  printf(1, "Attempting write to read-only page...\n");
  *p = 'A';
  printf(1, "This should not print\n");

  exit();
}

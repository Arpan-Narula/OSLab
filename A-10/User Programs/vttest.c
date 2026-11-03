#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  char *p;
  int va;

  p = sbrk(4096 * 2);

  va = (int)p;
  vtop(va);

  va = (int)p + 4096;
  vtop(va);

  va = (int)p + 100;
  vtop(va);

  va = 0x50000000;
  vtop(va);

  exit();
}

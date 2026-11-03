#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  char *p;

  memstats();
  p = sbrk(4096 * 5);
  memstats();
  sbrk(-4096 * 5);
  memstats();

  exit();
}

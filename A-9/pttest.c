#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  vmprint();
  sbrk(4 * 4096);
  vmprint();
  exit();
}

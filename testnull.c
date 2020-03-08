#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

int
main(int argc, char *argv[])
{
  int *x = (int *) 0;
  printf(1, "X is: %x\n", *x);

  exit();
}

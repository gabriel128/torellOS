#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

int
main(int argc, char *argv[])
{
  // Override the memory with the Codew ith FFs
  int *x;
  int *limit = (int *) (0x1010);

  for(x= (int *) 0x1000; x < limit; x++) {
    printf(1, "X was: %x\n", *x);
    *x=0xffffffff;
    printf(1, "X is: %x\n", *x);
  }

  exit();
}

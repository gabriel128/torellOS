#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

/* #define N  10 */


int
main(int argc, char *argv[])
{

  int i;
  for(i = 0; i < 4; i++){
    int rc2;
    rc2= fork();
    if (rc2 == 0) {
      settickets(80);
      int rc;
      rc= fork();
      if (rc == 0) {
        settickets(20);
      }
      wait();
    }
  }

  wait();
  exit();
}

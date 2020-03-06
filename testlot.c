#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

#define N  5


int
main(int argc, char *argv[])
{

  int i;
  for(i = 0; i < N; i++){
    int rc;
    rc = fork();

    if (rc == 0) {
      if(i % 2 == 0) {
        settickets(80);
      } else {
        settickets(20);
      }
    }
  }

  for(i = 0; i < (N); i++){
    wait();
  }

  exit();
}

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

int
main(int argc, char *argv[])
{

  int fd = 0;
  struct dirent de;

  read(fd, &de, sizeof(de));
  read(fd, &de, sizeof(de));
  read(fd, &de, sizeof(de));
  /* printf(1, "%d\n", getreadcount()); */
  /* printf(1, "%s\n", "sadfasdf"); */

  exit();
}

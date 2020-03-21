/* clone copies file descriptors, but doesn't share */
#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "x86.h"

#undef NULL
#define NULL ((void*)0)

#define PGSIZE (4096)

int ppid;
volatile uint newfd = 0;
volatile uint global = 0;

#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   kill(ppid); \
   exit(); \
}

void worker(void *arg_ptr);

int
main(int argc, char *argv[])
{
   ppid = getpid();

   int fd = open("tmp", O_WRONLY|O_CREATE);
   assert(fd == 3);

   int clone_pid = thread_create(worker, 0);
   int clone_pid2 = thread_create(worker, 0);
   printf(1,"clone pid=%d\n",clone_pid);
   printf(1,"clone pid2=%d\n",clone_pid2);
   assert(clone_pid > 0);
   while(!newfd);
   assert(write(newfd, "goodbye\n", 8) == -1);
   printf(1, "TEST PASSED\n");
   printf(1, "Global = %d\n", global);
   exit();
}

void
worker(void *arg_ptr) {
   assert(write(3, "hello\n", 6) == 6);

   int i;
   for(i = 0; i <10000000; i++)
     global++;
   xchg(&newfd, open("tmp2", O_WRONLY|O_CREATE));
   printf(1, "HERE\n");
   exit();
}

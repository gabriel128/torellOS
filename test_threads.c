#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "x86.h"

#undef NULL
#define NULL ((void*)0)

#define PGSIZE (4096)
#define assert(x) if (x) {} else {              \
    printf(1, "%s: %d ", __FILE__, __LINE__);   \
    printf(1, "assert failed (%s)\n", # x);     \
    printf(1, "TEST FAILED\n");                 \
    kill(ppid);                                 \
    exit();                                     \
  }

int ppid;
int *global;
int loops = 500;

lock_t *lock;
lock_t *ticket_lock;

void
no_lock_worker(void *arg_ptr) {
  sleep(2);
  int i, tmp;

  for (i = 0; i < loops; i++) {
    tmp = *global;
    sleep(1);
    int x = 1 +2;
    x = 1 +2;
    x = 1 +2;
    x = 1 +2;
    ppid = x;
    tmp++;
    *global = tmp;
  }
  exit();
}

void
lock_worker(void *arg_ptr) {
  acquire_mutex_lock(lock);
  int i, tmp;

  for (i = 0; i < loops; i++) {
    tmp = *global;
    sleep(1);
    tmp++;
    *global = tmp;
  }
  release_mutex_lock(lock);
  exit();
}

void
ticket_lock_worker(void *arg_ptr) {
  acquire_ticket_lock(ticket_lock);
  int i, tmp;

  for (i = 0; i < loops; i++) {
    tmp = *global;
    sleep(1);
    tmp++;
    *global = tmp;
  }

  release_ticket_lock(ticket_lock);
  exit();
}

int
main(int argc, char *argv[])
{
  ppid = getpid();

  global = malloc(sizeof(int));
  *global = 0;

  int a = 1;
  int b = 2;
  int c = 3;
  int d = 4;

  int thread_pid = thread_create(no_lock_worker, &a);
  int thread_pid2 = thread_create(no_lock_worker, &b);
  int thread_pid3 = thread_create(no_lock_worker, &c);
  int thread_pid4 = thread_create(no_lock_worker, &d);
  assert(thread_pid > 0);
  assert(thread_pid2 > 0);
  assert(thread_pid3 > 0);
  assert(thread_pid4 > 0);

  int j;
  for (j = 0; j < 4; j++) {
    int join_pid = thread_join();
    assert(join_pid > 0);
  }
  assert(*global != loops*4);

  printf(1, "Global ended up being %d\n", *global);


  *global = 0;

  lock = init_lock();

  thread_pid = thread_create(lock_worker, &a);
  thread_pid2 = thread_create(lock_worker, &b);
  thread_pid3 = thread_create(lock_worker, &c);
  thread_pid4 = thread_create(lock_worker, &d);
  assert(thread_pid > 0);
  assert(thread_pid2 > 0);
  assert(thread_pid3 > 0);
  assert(thread_pid4 > 0);

  for (j = 0; j < 4; j++) {
    int join_pid = thread_join();
    assert(join_pid > 0);
  }

  printf(1, "Global ended up being %d\n", *global);
  assert(*global == loops*4);

  *global = 0;

  ticket_lock = init_lock();

  thread_pid = thread_create(ticket_lock_worker, &a);
  thread_pid2 = thread_create(ticket_lock_worker, &b);
  thread_pid3 = thread_create(ticket_lock_worker, &c);
  thread_pid4 = thread_create(ticket_lock_worker, &d);
  assert(thread_pid > 0);
  assert(thread_pid2 > 0);
  assert(thread_pid3 > 0);
  assert(thread_pid4 > 0);

  for (j = 0; j < 4; j++) {
    int join_pid = thread_join();
    assert(join_pid > 0);
  }

  assert(*global == loops*4);
  printf(1, "Global ended up being %d\n", *global);

  printf(1, "Tests passed \n");
  exit();
}

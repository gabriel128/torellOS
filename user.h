#include "types.h"

struct stat;
struct rtcdate;

typedef struct __lock_t {
  uint locked;
  int turn;
  int ticket;
} lock_t;

// system calls
int fork(void);
int exit(void) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
int settickets(int);
int nap(void);
int clone(void(*func)(void *), void *arg1, void *stack);
int join(void **stack);

// ulib.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void printf(int, const char*, ...);
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint);
int thread_create(void (*start_routine)(void*), void *arg);
int thread_join();
lock_t* init_lock(void);
void acquire_mutex_lock(lock_t *lock);
void release_mutex_lock(lock_t *lock);
void acquire_ticket_lock(lock_t *lock);
void release_ticket_lock(lock_t *lock);
void free(void*);
int atoi(const char*);

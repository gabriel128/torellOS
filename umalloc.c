#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"
#include "x86.h"
#include "traps.h"
#include "fcntl.h"

#define PGSIZE 4096
// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

typedef long Align;

union header {
  struct {
    union header *ptr;
    uint size;
  } s;
  Align x;
};

typedef union header Header;

static Header base;
static Header *freep;

void
free(void *ap)
{
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  if(bp + bp->s.size == p->s.ptr){
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if(p + p->s.size == bp){
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
}

static Header*
morecore(uint nu)
{
  char *p;
  Header *hp;

  if(nu < 4096)
    nu = 4096;
  p = sbrk(nu * sizeof(Header));
  if(p == (char*)-1)
    return 0;
  hp = (Header*)p;
  hp->s.size = nu;
  free((void*)(hp + 1));
  return freep;
}

void*
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
  if((prevp = freep) == 0){
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
      if(p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      else {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
      if((p = morecore(nunits)) == 0)
        return 0;
  }
}

int
thread_create(void(*func)(void *), void *arg) {
  void *stack = malloc(PGSIZE*2);

  if (stack == (void*)0)
    return -1;

  if((uint)stack % PGSIZE)
    stack = stack + (PGSIZE - (uint)stack % PGSIZE);

  return clone(func, arg, stack);
}


int
thread_join() {
  void *stack = malloc(sizeof(void*));
  int result = join(&stack);

  free(stack);

  return result;
}

lock_t*
init_lock() {
  lock_t *lock = malloc(sizeof(lock_t));
  lock->locked = 0;
  lock->ticket = 0;
  lock->turn = 0;
  return lock;
}

void acquire_mutex_lock(lock_t *lock) {
  while(xchg(&lock->locked, 1) != 0)
     yieldcpu(); // Yield to avoid unnecessary spinning */
    ;
};

void release_mutex_lock(lock_t *lock) {
  lock->locked = 0;
};

void acquire_ticket_lock(lock_t *lock) {
  int myturn = fetch_and_add(&lock->ticket, 1);

  while(lock->turn != myturn)
    yieldcpu(); // Yield to avoid unnecessary spinning
    ;
}

void release_ticket_lock(lock_t *lock) {
  lock->turn = lock->turn + 1 ;
}

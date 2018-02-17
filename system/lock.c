#include <xinu.h>

void bwf_init (bwf_lock_t *l) {
  l->taken = 0;
  l->guard = 0;
  l->twlist = newqueue();
}

void bwf_lock (bwf_lock_t *l) {
  struct procent *prptr;
  prptr = &proctab[currpid];

  while (test_and_set(&l->guard, 1) == 1);
  
  if (l->taken == 0) {
    l->taken = 1;
    l->guard = 0;
  } else {
    insert(currpid,l->twlist,1);
    l->guard = 0;
    prptr->prstate = PR_WAIT;
    yield();
    l->taken = 1;
  }
}

void bwf_unlock(bwf_lock_t *l) {
  pid32 pid;
 
  while (test_and_set(&l->guard, 1) == 1);
    l->taken = 0;

  if(isempty(l->twlist)) {

    l->guard = 0;
    yield();
  }
  else {
    pid = dequeue(l->twlist);
    l->guard = 0;
    ready(pid);
  }
}

//midterm solution below, didn't use
/* 
void bwf_lock (bwf_lock_t *l) {
  intmask mask;
  struct procent *prptr;
  qid16 q;
  int present;

  while(test_and_set(&l->taken,1)==1) {
    mask = disable();

    present = 0;
    if(nonempty(l->twlist)) {
      q = firstid(l->twlist);
      while(q<lastid(l->twlist)) {
        if (queuetab[q].qkey==currpid) present = 1;
        q = queuetab[q].qnext;
      }
    }
    if (present==0) {
      prptr = &proctab[currpid];
      insert(currpid,l->twlist,1);
      kprintf("reached %d\n",currpid);
      prptr->prstate = PR_SUSP;
    }
    restore(mask);
  }     
}

void bwf_unlock (bwf_lock_t *l) {
  intmask mask;
  struct procent *prptr;
  pid32 pid;

  mask = disable();
  if (nonempty(l->twlist)) {
    pid = dequeue(l->twlist);
    kprintf("unreached %d\n",pid);
    prptr = &proctab[pid]; 
    prptr->prstate = PR_READY;
  }
  l->taken = 0;
  restore(mask);
}
*/

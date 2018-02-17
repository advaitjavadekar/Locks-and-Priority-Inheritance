#include <xinu.h>

void al_init (al_lock_t *l) {
  l->taken = 0;
  l->guard = 0;
  l->twlist = newqueue();
}

void al_lock (al_lock_t *l) {
  struct procent *prptr;
  struct procent *prloop;
  struct procent *prlast;
  uint32 i, j, check_done, iter_count;
  prptr = &proctab[currpid];

  while (test_and_set(&l->guard, 1) == 1);
  if (l->taken == 0) {
    prptr->locks[prptr->index] = l->twlist; //saving lock qid as latest lock acquired by a process
    prptr->index++;
    l->taken = 1;
    prptr->waitlock = 0;
    l->guard = 0;
  } else {
    prptr->waitlock = l->twlist;
    check_done = 0;
    iter_count = 0;
    
    prlast = prptr; 

    while(check_done != 1 && prlast->checked == 0 && iter_count<NPROC) {
      iter_count++;
      prlast->checked++;
      for(i = 0; i < NPROC; i++) {
        prloop = &proctab[i];
        if(prloop->index>0) {
          for(j = 0; j < prloop->index; j++) {
            if(prloop->locks[j] == prlast->waitlock) {
              if(currpid==i) kprintf("Deadlock detected!!\n");
              //kprintf("%d finds %d holds %d\n",currpid,i,j);
              if(prloop->prstate == PR_FREE) {      //deadlock case: Process holding lock is killed
                kprintf("\nDeadlock detected!\n");
                check_done = 1;
              }
              else if(prloop->prstate == PR_WAIT && prloop->waitlock!=0) { 
                prlast = prloop; 
                break;                 //lock dependency chain continues  
              }  
              else check_done = 1; // latest process checked is not waiting on any lock, so no deadlock
            }
          }
        }
      }
    }

    //if(prptr==prloop) //deadlock case: loop of locks, 2 or more processes involved  
    //  kprintf("Deadlock detected!\n");

    for(i = 0; i < NPROC; i++) {
      prloop = &proctab[i];
      prloop->checked = 0;
    }
    insert(currpid,l->twlist,1);         
    l->guard = 0;
    prptr->prstate = PR_WAIT;
    yield();

    l->taken = 1;
    prptr->locks[prptr->index] = l->twlist;
    prptr->index++;
    prptr->waitlock = 0;
  } 
}

void al_unlock (al_lock_t *l) {
  pid32 pid;
  struct procent *prptr;
  
  while (test_and_set(&l->guard, 1) == 1);
  l->taken = 0;
  prptr = &proctab[currpid];
  prptr->index--;
  if(isempty(l->twlist)) {
    l->guard = 0;
    yield();
  }
  else {
    pid = dequeue(l->twlist);
    //proctab[pid].locks[proctab[pid].index] = l->twlist;
    //proctab[pid].index++;
    l->guard = 0;
    ready(pid);
  }  
}  

bool8 al_trylock(al_lock_t *l) {
  struct procent *prptr;
  prptr = &proctab[currpid];   

  while (test_and_set(&l->guard, 1) == 1);
  if (l->taken == 0) {
    prptr->locks[prptr->index] = l->twlist; //saving lock qid as latest lock acquired by a process
    prptr->index++;
    l->taken = 1;
    prptr->waitlock = 0;
    l->guard = 0;
    return 1;
  }
  l->guard = 0;
  return 0;
}

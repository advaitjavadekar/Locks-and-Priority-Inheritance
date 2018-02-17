#include <xinu.h>

void pi_init (pi_lock_t *l) {
  l->pi_lock = 0;
  l->pi_guard = 0;
  l->pi_blist = newqueue();
}

void pi_lock (pi_lock_t *l) {
  struct procent *prptr;
  struct procent *prloop;
  struct procent *prlast;
  uint32 i, j, check_done, iter_count;
  prptr = &proctab[currpid];
  while (test_and_set(&l->pi_guard, 1) == 1);
  if (l->pi_lock == 0) {
    prptr->locks[prptr->index] = l->pi_blist; //saving lock qid as latest lock aquired by a process
//save priority in index
    prptr->prio_for_locks[prptr->index]=prptr->prprio;	     
    prptr->index++;
    l->pi_lock = 1;
    prptr->waitlock = 0;
    l->pi_guard = 0;
  } else {
    prptr->waitlock = l->pi_blist;

    check_done = 0;
    iter_count = 0;
    prlast = prptr; 

    while(check_done != 1 && prlast->checked ==0 && iter_count<NPROC) {
      iter_count++;
      prlast->checked++;
      for(i = 0; i < NPROC; i++) {
        prloop = &proctab[i];
        if(prloop->index>0) {
          for(j = 0; j < prloop->index; j++) {
            if(prloop->locks[j] == prlast->waitlock) {
              if(prloop->prstate == PR_FREE) {      //deadlock case: Process holding lock is killed
                kprintf("\nDeadlock detected!\n");
                check_done = 1;
              }
              else if(prloop->prstate == PR_WAIT && prloop->waitlock!=0) 
                {
		if(prloop->prio_for_locks[j]<prlast->prprio){
			prloop->prio_for_locks[j]=prlast->prprio;
			getitem(i);
			prloop->prprio=prlast->prprio;
			insert(i,l->pi_blist,prloop->prprio);
		} 
		//if prloops's priority[index] is less than prlast then priority[index] = prlast 
		//update prloop->prprio who holds the lock
		//getitem[prloop] ---> reinsert with updated priority
		prlast = prloop;

		 break;}  //lock dependency chain continues
              else {					//it means that proc[i] is holding the lock and currently executing
		check_done = 1;
		if(prloop->prio_for_locks[j]<prlast->prprio){
			prloop->prio_for_locks[j]=prlast->prprio;
			prloop->prprio=prlast->prprio;
			
		}
		//if prloops's priority[index] is less than prlast then priority[index] = prlast 
		//update prloop->prprio who holds the lock
		}

            }
          }
        }
      }
    }

    if(check_done == 0) //deadlock case: loop of locks, 2 or more processes involved  
      kprintf("\nDeadlock detected! huh\n");

    for(i = 0; i < NPROC; i++) {
      prloop = &proctab[i];
      prloop->checked = 0;
    }
    insert(currpid,l->pi_blist,&proctab[currpid].prprio);         
    l->pi_guard = 0;
    prptr->prstate = PR_WAIT;
    yield();
    l->pi_lock = 1;
    prptr->locks[prptr->index] = l->pi_blist;
    prptr->index++;
    prptr->waitlock = 0;
    
  } 
}

void pi_unlock (pi_lock_t *l) {
  pid32 pid;
  struct procent *prptr;
  
  while (test_and_set(&l->pi_guard, 1) == 1);
  l->pi_lock = 0;
  prptr = &proctab[currpid];
  prptr->index--;
  if(prptr->index>0){
	prptr->prprio = prptr->prio_for_locks[prptr->index-1];
  }
  else 
	prptr->prprio = prptr->orig_prio;  
if(isempty(l->pi_blist)) {
    l->pi_guard = 0;
    yield();
  }
  else {
    pid = dequeue(l->pi_blist);
    l->pi_guard = 0;
    ready(pid);
  }  
}  





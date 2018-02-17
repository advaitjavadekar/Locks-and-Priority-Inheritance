/*  main.c  - main */

#include <xinu.h>

uint32 sum;
al_lock_t l1, l2, l3;
intmask mask;
/* each thread trying to add its input parameter to global sum
   i.e., sum = a+b+c */

//Case 1: three threads without trylock, possible deadlock situation 
void deadlock1(uint32 a) {  
  al_lock(&l1);
  mask = disable(); kprintf("P1 acquired lock l1\n"); restore(mask);
  sleepms(10);
  mask = disable(); kprintf("P1 tried acquiring lock l2\n"); restore(mask);
  al_lock(&l2);
  al_lock(&l3);
  sum = sum + a;
  al_unlock(&l3);
  al_unlock(&l2);
  al_unlock(&l1);
}

void deadlock2(uint32 b) {
  al_lock(&l2);
  mask = disable(); kprintf("P2 acquired lock l2\n"); restore(mask);
  sleepms(20);
  mask = disable(); kprintf("P2 tried acquiring lock l3\n"); restore(mask);
  al_lock(&l3);
  al_lock(&l1);
  sum = sum + b;
  al_unlock(&l1);
  al_unlock(&l3);
  al_unlock(&l2);
}

void deadlock3(uint32 c) {
  al_lock(&l3);
  mask = disable(); kprintf("P3 acquired lock l3\n"); restore(mask);
  sleepms(40);
  mask = disable(); kprintf("P3 tried acquiring lock l1\n"); restore(mask);
  al_lock(&l1);
  al_lock(&l2);
  sum = sum + c;
  al_unlock(&l2);
  al_unlock(&l1);
  al_unlock(&l3);
} 

//-------------------------------------------------------------------

//Case 2: three threads with trylock 
void trylock1(uint32 a) {
top1:
  al_lock(&l1);
  sleepms(2);
  if(al_trylock(&l2)==0) {
    al_unlock(&l1);
    goto top1;
  }
  if(al_trylock(&l3)==0) {
    al_unlock(&l2);
    al_unlock(&l1);
    goto top1;
  }
  mask = disable(); kprintf("P1 acquired all locks\n"); restore(mask);
  sum = sum + a;
  al_unlock(&l3);
  al_unlock(&l2);
  al_unlock(&l1);
  mask = disable(); kprintf("P1 leaves all locks\n"); restore(mask);
}

void trylock2(uint32 b) {
top2:
  al_lock(&l2);
  sleepms(3);
  if(al_trylock(&l3)==0) {
    al_unlock(&l2);
    goto top2;
  }
  if(al_trylock(&l1)==0) {
    al_unlock(&l3);
    al_unlock(&l2);
    goto top2;
  }
  mask = disable(); kprintf("P2 acquired all locks\n"); restore(mask);
  sum = sum + b;
  al_unlock(&l1);
  al_unlock(&l3);
  al_unlock(&l2);
  mask = disable(); kprintf("P2 leaves all locks\n"); restore(mask);
}

void trylock3(uint32 c) {
top3:
  al_lock(&l3);
  sleepms(4);
  if(al_trylock(&l1)==0) {
    al_unlock(&l3);
    goto top3;
  }
  if(al_trylock(&l2)==0) {
    al_unlock(&l1);
    al_unlock(&l3);
    goto top3;
  }  
  mask = disable(); kprintf("P3 acquired all locks\n"); restore(mask);
  sum = sum + c;
  al_unlock(&l2);
  al_unlock(&l1);
  al_unlock(&l3);
  mask = disable(); kprintf("P3 leaves all locks\n"); restore(mask);
}

//-------------------------------------------------------------------

process	main(void)
{
  pid32 pid1, pid2, pid3;
  int a,b,c;
  a = 10;
  b = 20;
  c = 30;
  kprintf("\nSum expected value  = %d\n",a+b+c);


  kprintf("\nCase 1: threads without trylock\n");
  al_init(&l1);
  al_init(&l2);
  al_init(&l3);
  sum = 0;
  pid1 = create(deadlock1,1024,15,"deadlock1",1,a); 
  pid2 = create(deadlock2,1024,15,"deadlock2",1,b); 
  pid3 = create(deadlock3,1024,15,"deadlock3",1,c); 
  resume(pid1);
  resume(pid2);
  resume(pid3);
  sleepms(5000);
  kprintf("Sum without trylock = %d\n",sum);


  kill(pid1);     // kill needed to get the processes out of deadlock
  kill(pid2);     // and reuse them for trylock case
  kill(pid3);


  kprintf("\nCase 2: threads with trylock\n");
  al_init(&l1);
  al_init(&l2);
  al_init(&l3);
  sum = 0;
  pid1 = create(trylock1,1024,15,"trylock1",1,a); 
  pid2 = create(trylock2,1024,15,"trylock2",1,b); 
  pid3 = create(trylock3,1024,15,"trylock3",1,c); 
  resume(pid1);
  resume(pid2);
  resume(pid3);
  sleepms(5000);
  kprintf("Sum with trylock    = %d\n",sum);



                  
  /* DEFAULT PROGRAM TO START SHELL */
  /* Run the Xinu shell */

	recvclr();
	resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	/* Wait for shell to exit and recreate it */

	while (TRUE) {
		receive();
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}
	return OK;
    
}


/*  main.c  - main */

#include <xinu.h>

uint32 sum;
uint32 array[100];
uint32 thcount;
bwf_lock_t l,l2;
pi_lock_t pl,pl2;
uint32 stime1,etime1,stime2,etime2,stime3,etime3,stime4,etime4,stime5,etime5,stime6,etime6;
uint32 stime7,etime7,stime8,etime8,stime9,etime9,stime10,etime10,stime11,etime11,stime12,etime12;	
pid32 pid1,pid2,pid3,pid4,pid5,pid6,pid7,pid8,pid9,pid10,pid11,pid12; 

/* fake array function to virtually make the index larger */
uint32 arrayf(uint32 index) {
  return array[index%100];
}

//------------------------------------------------------------------

// CASE1: EFFECTIVE PRIORITY INVERSION

/* 3 threads that perform addition with priority inheritance locks*/
void p1() {
  uint32 i;
	pid1=currpid;
    pi_lock(&pl);
    for(i=0; i<100000000; i++) {
    sum = sum + arrayf(i);
	}
  etime1=ctr1000;
    pi_unlock(&pl);

  kill(currpid);
}	

void p2() {
  uint32 i;
  	pid2=currpid;
    pi_lock(&pl);
    for(i=0; i<100000000; i++) {
    sum = sum + arrayf(i);
	}
  etime2=ctr1000;
    pi_unlock(&pl);

  kill(currpid);
}	

void p3() {
  uint32 i;
  	pid3=currpid;
    pi_lock(&pl);
    for(i=0; i<100000000; i++) {
    sum = sum + arrayf(i);
	}
  etime3=ctr1000;
    pi_unlock(&pl);

  kill(currpid);
}	

/* 3 threads that perform addition with bwf queue locks*/
void p4() {
  uint32 i;
  	pid4=currpid;
    bwf_lock(&l);
    for(i=0; i<100000000; i++) {
    sum = sum + arrayf(i);
	}
  etime4=ctr1000;
    bwf_unlock(&l);

  kill(currpid);
}	

void p5() {
  uint32 i;
  	pid5=currpid;
    bwf_lock(&l);
    for(i=0; i<100000000; i++) {
    sum = sum + arrayf(i);
	}
  etime5=ctr1000;
    bwf_unlock(&l);

  kill(currpid);
}	

void p6() {
  uint32 i;
  	pid6=currpid;
    bwf_lock(&l);
    for(i=0; i<100000000; i++) {
    sum = sum + arrayf(i);
	}
  etime6=ctr1000;
    bwf_unlock(&l);

  kill(currpid);
}	

//-------------------------------------------------------------------

//CASE 2: CHAIN OF BLOCKING CASE

/* 3 threads that perform addition with priority inheritance locks*/
void p7() {
  uint32 i;
	pid7=currpid;
    pi_lock(&pl);
    for(i=0; i<100000000; i++) {
    sum = sum + arrayf(i);
	}
  etime7=ctr1000;
    pi_unlock(&pl);

  kill(currpid);
}	

void p8() {
  uint32 i;
  	pid8=currpid;
    pi_lock(&pl2);
    for(i=0; i<100000000; i++) {
    sum = sum + arrayf(i);
	}
  etime8=ctr1000;
    pi_unlock(&pl2);

  kill(currpid);
}	

void p9() {
  uint32 i;
  	pid9=currpid;
    pi_lock(&pl);
    pi_lock(&pl2);
    for(i=0; i<100000000; i++) {
    sum = sum + arrayf(i);
	}
  etime9=ctr1000;
    pi_unlock(&pl2);
    pi_unlock(&pl);


  kill(currpid);
}	

/* 3 threads that perform addition with locks with bwf queue locks*/
void p10() {
  uint32 i;
  	pid10=currpid;
    bwf_lock(&l);
    for(i=0; i<100000000; i++) {
    sum = sum + arrayf(i);
	}
  etime10=ctr1000;
    bwf_unlock(&l);

  kill(currpid);
}	

void p11() {
  uint32 i;
  	pid11=currpid;
    bwf_lock(&l2);
    for(i=0; i<100000000; i++) {
    sum = sum + arrayf(i);
	}
  etime11=ctr1000;
    bwf_unlock(&l2);

  kill(currpid);
}	

void p12() {
  uint32 i;
  	pid12=currpid;
    bwf_lock(&l);
    bwf_lock(&l2);
    for(i=0; i<100000000; i++) {
    sum = sum + arrayf(i);
	}
  etime12=ctr1000;
    bwf_unlock(&l2);
    bwf_unlock(&l);

  kill(currpid);
}

//-------------------------------------------------------------------------

process	main(void)
{
  pi_init(&pl);
  pi_init(&pl2);
  bwf_init(&l);
  bwf_init(&l2);

  kprintf("\n\n\nWait.. calculations may take several seconds\n");  
	
  resume(create(p1,1024,2,"p1",0));
	stime1=ctr1000;
  sleepms(2);	
  resume(create(p2,1024,4,"p2",0));
	stime2=ctr1000;
  sleepms(2);
  resume(create(p3,1024,6,"p3",0));
	stime3=ctr1000;
  sleepms(10000);
  resume(create(p4,1024,2,"p4",0));
	stime4=ctr1000;
  sleepms(2);	
  resume(create(p5,1024,4,"p5",0));
	stime5=ctr1000;
  sleepms(2);
  resume(create(p6,1024,6,"p6",0));
	stime6=ctr1000;
  sleepms(10000);	

kprintf("\n\n\nCASE 1: Priority Inversion Allows Efficient Execution\n");

	kprintf("\nP1 has priority 2 uses priority inheritance lock and execution time=%d",etime1-stime1);
	kprintf("\nP2 has priority 4 uses priority inheritance lock and execution time=%d",etime2-stime2);
	kprintf("\nP3 has priority 6 uses priority inheritance lock and execution time=%d <--highest priority",etime3-stime3);

	kprintf("\n");

	kprintf("\nP4 has priority 2 uses bwf lock and execution time=%d",etime4-stime4);
	kprintf("\nP5 has priority 4 uses bwf lock and execution time=%d",etime5-stime5);
	kprintf("\nP6 has priority 6 uses bwf lock and execution time=%d <--highest priority\n",etime6-stime6);


//PROCESSES WITH CHAIN OF BLOCKING CASE

  resume(create(p7,1024,2,"p7",0));
	stime7=ctr1000;
  sleepms(2);	
  resume(create(p8,1024,4,"p8",0));
	stime8=ctr1000;
  sleepms(2);
  resume(create(p9,1024,6,"p9",0));
	stime9=ctr1000;
  sleepms(10000);
  resume(create(p10,1024,2,"p10",0));
	stime10=ctr1000;
  sleepms(2);	
  resume(create(p11,1024,4,"p11",0));
	stime11=ctr1000;
  sleepms(2);
  resume(create(p12,1024,6,"p12",0));
	stime12=ctr1000;
  sleepms(10000);	
	
  kprintf("\n\n\nCASE 2: Priority Inversion is limited by Chain of Blocking\n");

	kprintf("\nP1 has priority 2 uses priority inheritance lock and execution time=%d",etime7-stime7);
	kprintf("\nP2 has priority 4 uses priority inheritance lock and execution time=%d",etime8-stime8);
	kprintf("\nP3 has priority 6 uses priority inheritance lock and execution time=%d <--highest priority",etime9-stime9);

	kprintf("\n");

	kprintf("\nP4 has priority 2 uses bwf lock and execution time=%d",etime10-stime10);
	kprintf("\nP5 has priority 4 uses bwf lock and execution time=%d",etime11-stime11);
	kprintf("\nP6 has priority 6 uses bwf lock and execution time=%d <--highest priority\n",etime12-stime12);



  /* DEFAULT PROGRAM TO START SHELL */
  /* Run the Xinu shell */

	recvclr();
	resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	/* Wait for shell to exit and recreate it */

	while (TRUE) {
		receive();
		sleepms(200);
		kprintf("\n\n\nMain process recreating shell\n\n");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}
	return OK;
    
}


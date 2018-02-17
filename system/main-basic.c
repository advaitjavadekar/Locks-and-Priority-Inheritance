/*  main.c  - main */

#include <xinu.h>

uint32 sum;
uint32 array[100];
bwf_lock_t l;

/* fake array function to virtually make the index larger */
uint32 arrayf(uint32 index) {
  return array[index%100];
}

/* single thread that performs addition without locks */
void add_nolock(uint32 *array, uint32 start_index, uint32 stepsize, uint32 max_index) {
  uint32 i;
  for(i=start_index; i<max_index; i=i+stepsize) {
    sum = sum + arrayf(i);
  }
  kill(currpid);
}

/* single thread that performs addition with locks */
void add_lock(uint32 *array, uint32 start_index, uint32 stepsize, uint32 max_index) {
  uint32 i;
  for(i=start_index; i<max_index; i=i+stepsize) {
    bwf_lock(&l);
    sum = sum + arrayf(i);
    bwf_unlock(&l);
  }
  kill(currpid);
}

/* performs the summation using naive threads */
uint32 naive_parallel_summation (uint32 *array, uint32 n, uint32 num_threads) {
  int m;
  sum=0;
  for(m=0; m<num_threads; m++)
    resume(create(add_nolock,1024,15,"no_lock",4,array, m, num_threads, n));
  sleepms(3000);
  return sum;
}

/* performs the summation using lock-based threads */
uint32 sync_parallel_summation (uint32 *array, uint32 n, uint32 num_threads) {
  int m;
  sum=0;
  for(m=0; m<num_threads; m++)
    resume(create(add_lock,1024,15,"with_lock",4,array, m, num_threads, n));
  sleepms(90000);
  return sum;
}

/* performs the summation serially (one thread) */
uint32 serial_summation(uint32 *array, uint32 n) {
  uint32 i;
  sum=0;
  for(i=0; i<n; i++)
    sum += arrayf(i);
  return sum;
}  
  
process	main(void)
{
  int i;
  uint32 ser_sum, par_sum_nolock, par_sum_lock;
  
  for(i=0; i<100; i++) 
    array[i] = 1;       
  
  bwf_init(&l);

  kprintf("\n\n\nWait.. calculations may take upto 2 minutes");

  ser_sum = serial_summation(array, 300000000);
  kprintf("\nSerial Summation = %d",ser_sum);
  par_sum_nolock = naive_parallel_summation(array, 300000000, 20);
  kprintf("\nNaive Parallel Summation = %d",par_sum_nolock);
  par_sum_lock = sync_parallel_summation(array, 300000000, 20);
  kprintf("\nSync Parallel Summation = %d",par_sum_lock);




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


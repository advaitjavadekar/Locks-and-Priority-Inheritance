/*  main.c  - main */

#include <xinu.h>

uint32 sum;
uint32 array[100];
uint32 thcount;
bwf_lock_t l;
sl_lock_t sl;
uint32 stime1;
uint32 etime1;
uint32 stime2;
uint32 etime2;	

/* fake array function to virtually make the index larger */
uint32 arrayf(uint32 index) {
  return array[index%100];
}

/* single thread that performs addition with locks with spinlocks*/
void add_sl_lock(uint32 *array, uint32 start_index, uint32 stepsize, uint32 max_index) {
  uint32 i;
  	
  for(i=start_index; i<max_index; i=i+stepsize) {
    sl_lock(&sl);
    sum = sum + arrayf(i);
    sl_unlock(&sl);
  }
  etime1=ctr1000;
  kill(currpid);
}	

/* single thread that performs addition with locks without busy waiting*/
void add_lock(uint32 *array, uint32 start_index, uint32 stepsize, uint32 max_index) {
  uint32 i;
  for(i=start_index; i<max_index; i=i+stepsize) {
    bwf_lock(&l);
    sum = sum + arrayf(i);
    bwf_unlock(&l);
  }
  etime2=ctr1000;	
  kill(currpid);
}

/* performs the summation using spinlock threads */
uint32 sl_parallel_summation (uint32 *array, uint32 n, uint32 num_threads) {
  int m;
  sum=0;
  for(m=0; m<num_threads; m++)
    resume(create(add_sl_lock,1024,15,"sl_lock",4,array, m, num_threads, n)); 
  stime1=ctr1000;        
  sleepms(20000);

  return sum;
}

/* performs the summation using queue lock-based threads */
uint32 sync_parallel_summation (uint32 *array, uint32 n, uint32 num_threads) {
  int m;
  sum=0;
  for(m=0; m<num_threads; m++)
    resume(create(add_lock,1024,15,"with_queue_lock",4,array, m, num_threads, n));
  stime2=ctr1000;
  sleepms(10000);
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
  uint32 ser_sum, par_sum_sl_lock, par_sum_lock;
  
  for(i=0; i<100; i++) 
    array[i] = 1;      
  
  bwf_init(&l);

  kprintf("\n\n\nWait.. calculations may take several seconds\n\n");

  ser_sum = serial_summation(array, 20000000);
  kprintf("Serial Summation = %d\n",ser_sum);

  par_sum_sl_lock = sl_parallel_summation(array, 20000000, 30);
  kprintf("Spinlock Parallel Summation = %d\n",par_sum_sl_lock);
  kprintf("Spinlock Execution Time = %d\n",etime1-stime1);

  par_sum_lock = sync_parallel_summation(array, 20000000, 30);
  kprintf("Queue based lock Parallel Summation = %d\n",par_sum_lock);
  kprintf("Queue based lock Execution Time = %d\n",etime2-stime2);




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


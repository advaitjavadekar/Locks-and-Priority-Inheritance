/* test_and_set.S - test_and_set (x86) */

    .text
    .globl test_and_set

/*------------------------------------------------------------------------------------------
 * test_and_set -  X86 test_and_set; the call is test_and_set(uint32 *ptr, uint32 new_value)
 *------------------------------------------------------------------------------------------
 */

test_and_set:
		pushl	%ebp		      /* Push ebp onto stack */
		movl	%esp,%ebp	    /* Record current SP in ebp	*/ 
    cli                 /* Disable interrupts */  
    pushfl              /* Push flags onto stack */
    pushal              /* Push general regs. on stack */

    movl  8(%ebp),%ebx  /* get ptr location */
    movl  12(%ebp),%eax /* get new_value */
    xchg  (%ebx),%eax   /* exchange values */

    movl  %eax,12(%ebp) /* saving old_value before restoring regs. */ 
    /* since new_value is already written at *ptr with xchg , it's place on stack is useless */
    /* hence reused and stack location replaced with old_value, which is the function return */
    
    popal               /* Restore general registers */
    movl  12(%ebp),%eax /* Restore return value */
    movl  4(%esp),%ebp  /* Pick up ebp before restoring */

    popfl               /* Restore interrupt mask */
    add   $4,%esp       /* Skip saved value of ebp */ 
    sti

    ret                 /* Return */


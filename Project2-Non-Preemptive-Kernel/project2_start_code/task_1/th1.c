/*  Simple counter program, to check the functionality of kernel_entry(SYSCALL_SCHEDULER).
    Print time in seconds.

    Best viewed with tabs set to 4 spaces.
 */

#include "scheduler.h"
#include "th.h"
#include "util.h"

static void print_counter(void);

/*
 * This thread runs indefinitely
 */
void clock_thread(void)
{
	unsigned int time;
	unsigned int ticks,start_ticks;
	unsigned int start_time;
	int i;

  /* __asm__ __volatile__("break 111"); */

	/* To show time since last boot, remove all references to start_time */
	start_ticks = get_timer() ;  /* divide on 2^20 = 10^6 (1048576) */
			       /* divide on CPU clock frequency in
					     * megahertz */
	for ( i = 0; i < 10; ++i){
		ticks = get_timer();      /* divide on 2^20 = 10^6 (1048576) */
		ticks = (ticks - start_ticks);
		time = ((unsigned int) ticks) / MHZ;     /* divide on CPU clock frequency in
						 * megahertz */
		print_location(1, 1);
		printstr("Time (in us): ");
		printint(20, 1, time);
    /* __asm__ __volatile__("break 111"); */
		print_counter();
    /* __asm__ __volatile__("break 222"); */
		do_yield();
	}
	do_exit();
}

static void print_counter(void)
{
	static int counter = 0;

	print_location(1, 2);
	printstr("Thread 1 (time) : ");
	printint(20, 2, counter++);

}

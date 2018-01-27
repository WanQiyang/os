/* Simple counter program, to check the functionality of do_yield(). Print
   time in seconds.

   Best viewed with tabs set to 4 spaces. */

#include "common.h"
#include "kernel.h"
#include "scheduler.h"
#include "th.h"
#include "util.h"
#include "printf.h"

/*
 * This thread runs indefinitely, which means that the
 * scheduler should never run out of processes.
 */
void clock_thread(void)
{
    while (TRUE) {
        printf(24, 50, "Time (in seconds) : %d",
				(int) do_gettimeofday());
            //   (int) do_gettimeofday() / 1000);
//		print_status();
        do_yield();
    }
}

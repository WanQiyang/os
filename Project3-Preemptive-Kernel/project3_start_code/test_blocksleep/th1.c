#include "scheduler.h"
#include "printf.h"
#include "kernel.h"
#include "util.h"

#define SLEEP_TIME_SECONDS      (2)
#define TSC_PER_SECOND          (2000000)
#define MIN_COUNTERS_PER_SECOND (1000)

volatile int counter = 0;

void thread_1(void)
{
  printf(1,10, "Test 1: does scheduler survive when all processes are sleeping?");
  printf(2,10, "Thread 1 going to sleep");

  // both processes are asleep at this time
  do_sleep(500);

  printf(4,10, "Thread 1 waking up     ");

  for(;;)
  {
    counter++;
    do_yield();
  }
}

void thread_2(void)
{
  printf(3,10, "Thread 2 going to sleep for 4 seconds");

  // both processes are asleep at this time
  do_sleep(4000);

  printf(5,10, "Thread 2 waking up     ");

  printf(7,10, "Test 1 passed");

  printf(9,10, "Other 3 tests begin, please wait %d seconds", SLEEP_TIME_SECONDS);
  const uint64_t tsc_before = get_timer();
  const int entry_count_before = current_running->entry_count;
  counter = 0;
  do_sleep(SLEEP_TIME_SECONDS * 1000);
  const int entry_count_after = current_running->entry_count;
  const uint64_t tsc_after = get_timer();
  const int counter_after = counter;

  if( entry_count_before + 1 == entry_count_after )
  {
    printf(10,10, "(1) Entry count looks good...");
  }
  else if( entry_count_before + 3 > entry_count_after )
  {
    printf(10,10, "(1) Entry count test inconclusive... try again");
  }
  else
  {
    printf(10,10, "(1) Entry count looks **BAD**");
  }

  const int expected_change_tsc = SLEEP_TIME_SECONDS * TSC_PER_SECOND;
  const int observed_change_tsc = (int) (tsc_after - tsc_before);

  printf(11,10, "(2) do_sleep(%d*1000) lasted %d clock ticks; expected %d clock ticks.",
    SLEEP_TIME_SECONDS, observed_change_tsc,expected_change_tsc);

/* modify the rules for better judge
 * 10/08/2012
 */
  if( observed_change_tsc * 101 < expected_change_tsc *100 )
  {
    const int loss = 100 * (expected_change_tsc - observed_change_tsc)/expected_change_tsc;
    printf(12,10, "    maybe you are losing %d%% of the timer interrupts", loss);
  }
  else if( observed_change_tsc * 100 > expected_change_tsc * 101 )
  {
    printf(12,10, "    You slept more than 1%% too long... might be a problem.\n");
  }
  else
  {
    printf(12,10, "    Looks pretty good!");
  }

  if( counter_after < SLEEP_TIME_SECONDS*MIN_COUNTERS_PER_SECOND )
  {
    printf(13,10, "(3) Counter test gives bad results (%d/%d sec)", counter_after, SLEEP_TIME_SECONDS);
  }
  else
  {
    printf(13,10, "(3) Counter test looks good.");
  }

  for(;;)
    do_yield();
}



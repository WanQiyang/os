#include "common.h"
#include "scheduler.h"
#include "util.h"

int32_t ticks_TT = 0, ticks_2PT = 0;

void thread4(void)
{
  uint32_t i;
  ticks_TT -= get_timer();
  do_yield();
  for(i=1;i<100;i++) {
    ticks_TT -= get_timer();
    ticks_2PT += get_timer();
    do_yield();
  }
  ticks_2PT += get_timer();
  /* ticks_2PT /= 200; */
  ticks_2PT /= 2;
  uint32_t time = 0;
  time = ticks_2PT / MHZ;
  print_location(1, 2);
  printstr("100 P-T Switch Time (in us): ");
  printint(40, 2, time);
  do_exit();
}

void thread5(void)
{
  uint32_t i;
  for(i=0;i<100;i++) {
    ticks_TT += get_timer();
    ticks_2PT -= get_timer();
    do_yield();
  }
  /* ticks_TT /= 100; */
  uint32_t time = 0;
  time = ticks_TT / MHZ;
  print_location(1, 1);
  printstr("100 T-T Switch Time (in us): ");
  printint(40, 1, time);
  do_exit();
}

#include "kernel.h"
#include "scheduler.h"

void ps_thread(void)
{
  for(;;)
  {
    print_status();
    do_yield();
  }
}

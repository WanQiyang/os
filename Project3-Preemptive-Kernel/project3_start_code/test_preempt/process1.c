#include "printf.h"

void _start(void)
{
  int i;
  for(i=0; ;++i)
  {
    printf(2,1, "Process 1: %d", i);
  }
}


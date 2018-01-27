#include "syslib.h"

void _start(void)
{
  for(;;)
    yield();
}

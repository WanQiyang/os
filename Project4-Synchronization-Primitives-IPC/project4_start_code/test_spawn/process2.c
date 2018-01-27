/* Just "flies a plane" over the screen. */
#include "common.h"
#include "syslib.h"
#include "util.h"
#include "printf.h"


void __attribute__((section(".entry_function"))) _start(void)
{
  pid_t myPid = getpid();

  int myRow = myPid / 2;
  int myCol = 40* (myPid % 2);

  int i;
  for(i=0; ; ++i)
  {
    printf(myRow, myCol, "Count(%d): %d",
      myPid, i);
    sleep(10);
  }
}

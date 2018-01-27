#include "common.h"
#include "syslib.h"
#include "util.h"
#include "printf.h"

void init(void)
{
  ASSERT( spawn("clock_thread") >= 0 );
  ASSERT( spawn("SunQuan") >= 0 );
  ASSERT( spawn("LiuBei") >= 0 );
  ASSERT( spawn("CaoCao") >= 0 );
  exit();
}



#include "common.h"
#include "syslib.h"
#include "util.h"
#include "printf.h"

void CaoCao(void)
{
//  uint32_t myRand = 1234;
  uint32_t myRand = timer() + 1234;

  pid_t myPid = getpid();

  mbox_t subSunQuan = mbox_open("SunQuan-Publish-PID");
  mbox_t subLiuBei = mbox_open("LiuBei-Publish-PID");

  pid_t sunquan, liubei;

  mbox_recv(subSunQuan, &sunquan, sizeof(pid_t));
  mbox_recv(subLiuBei, &liubei, sizeof(pid_t));

  for(;;)
  {
    printf(7,1, "CaoCao(%d): I am working... muahaha ", myPid);

    sleep(5000);

    printf(7,1, "CaoCao(%d): I have my decision! ", myPid);

    myRand = rand_step(myRand);

    switch( myRand % 2 )
    {
      case 0:
        printf(8, 1, "CaoCao(%d): I will kill SunQuan (%d)!  ", myPid, sunquan);
        sleep(1000);
        printf(1,1, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ");
        kill( sunquan );
        mbox_recv(subSunQuan, &sunquan, sizeof(pid_t));
        printf(8, 1, "CaoCao(%d): Oops! SunQuan(%d) lives!                 ", myPid, sunquan);
        break;
      case 1:
        printf(9, 1, "CaoCao(%d): I will kill LiuBei(%d)! ", myPid, liubei);
        sleep(1000);
        printf(2,1, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ");
        kill( liubei );
        mbox_recv(subLiuBei, &liubei, sizeof(pid_t));
        printf(10, 1, "CaoCao(%d): Oops! LiuBei(%d) is alive again! ", myPid, liubei);
        break;
    }

    sleep(2000);
    printf(11, 1, "                                                           ");
    printf(12, 1, "                                                                      ");
  }
}

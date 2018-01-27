#include "common.h"
#include "syslib.h"
#include "util.h"
#include "printf.h"

void LiuBei(void)
{
  mbox_t pub = mbox_open("LiuBei-Publish-PID");
  pid_t myPid = getpid();

  /* Send PID twice, once for sunquan Hood,
   * and once for the CaoCao
   */
  mbox_send(pub, &myPid, sizeof(pid_t));
  mbox_send(pub, &myPid, sizeof(pid_t));

  /* Find sunquan's PID */
  mbox_t sub = mbox_open("SunQuan-Publish-PID");

  for(;;)
  {
    pid_t aramis;
    mbox_recv(sub, &aramis, sizeof(pid_t));

    printf(2,1, "LiuBei(%d): I'm waiting for SunQuan         ", myPid);

    wait(aramis);

    printf(2,1, "LiuBei(%d): I'm coming to save you, SunQuan!", myPid);

    sleep(1000);
    spawn("SunQuan");
    mbox_send(pub, &myPid, sizeof(pid_t));
  }

}


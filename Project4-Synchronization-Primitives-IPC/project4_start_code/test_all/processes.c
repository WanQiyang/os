#include "common.h"
#include "syslib.h"
#include "util.h"
#include "printf.h"
#include "processes.h"

/*
 * These are all of the programs which we include in our
 * ramdisk filesystem.
 *
 * It is VERY IMPORTANT that these functions do
 * not use any global variables.  This is because
 * each one might be running many times, and we do
 * not have any facility to duplicate their
 * data segments.
 *
 */

static void get_line(char *buffer, int maxlen);

/* The 'init' process is a shell
 * that lets you spawn other programs.
 */
int test_times = 0;
void init_process(void)
{
	spawn("barrier1");
	spawn("barrier2");
	spawn("barrier3");
	spawn("clock_thread");
	spawn("thread2");
	spawn("thread3");
	spawn("num");
	spawn("caps");
	spawn("scroll_th");
	spawn("PROC1");
	spawn("PROC2");
	exit();
}



#include "common.h"
#include "syslib.h"
#include "util.h"
#include "printf.h"

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
void init_process(void)
{
	spawn("PROC1");
	spawn("PROC2");
	exit();
}



#ifndef PROCESSES_H
#define PROCESSES_H

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

/* The 'init' process is a shell
 * that lets you spawn other programs.
 */
void init_process(void);

void barrier1(void);
void barrier2(void);
void barrier3(void);

#endif

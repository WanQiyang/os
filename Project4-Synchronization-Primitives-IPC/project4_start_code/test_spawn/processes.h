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

/* the help process prints help,
 * then exits
 */
void help_process(void);

/* The count process just counts
 * seconds since it started
 */
void count_process(void);

/* The producer process will
 * put numbers into a message
 * box.
 */
void producer_process(void);

/* The consumer will pull
 * numbers from a message box
 */
void consumer_process(void);

/* This process will do nothing
 * but shutdown
 */
void shutdown_process(void);

/* Draws the airplane */
void airplane_process(void);

#endif


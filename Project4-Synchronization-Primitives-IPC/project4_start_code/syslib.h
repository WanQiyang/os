/* syslib.h */
/* Do not change this file */

#ifndef SYSLIB_H
#define SYSLIB_H

// Includes
#include "common.h"

// Constants
enum {
    IGNORE = 0
};

/* yield the current process */
void yield(void);

/* terminate the current process */
void exit(void);

/* get the process id */
int getpid(void);

/* sleep for the specified number of milliseconds */
void sleep(int);

/* shut down the computer */
void shutdown(void);

/* write the character to the serial port
 * Note that bochs is configured so that
 * every character written to the serial
 * port is appended to the file serial.out
 */
void write_serial(char);

/* Extra Credit */
/* get the process priority */
int getpriority(void);

/* Extra Credit */
/* set the process priority
 * 1 - lowest priority
 * >1 - higher priority
 */
void setpriority(int);

char get_char(void);

/* start a new process from the given
 * name.
 * If the process name could not be found, return -1.
 * If the process table is full, return -2.
 * If successful, return the PID of the new process.
 *
 * Note that these a not really "filenames,"
 * but actually refer to names in the
 * ramdisk.  See 'ramdisk.h'
 */
pid_t spawn(const char *filename);

/* Kills a process; i.e. mark the process
 * as exited.  This should free-up the
 * entry in the process table, so that
 * later calls to spawn() can use that
 * process table entry.
 * Returns -1 if that process does not
 * exist, or 0 otherwise.
 */
int kill(pid_t pid);

/* Blocks until the specified process
 * terminates.  Returns -1 if that
 * process does not exist.
 * Returns 0 otherwise.
 */
int wait(pid_t pid);

void open_lock(lock_t* l);

int acquire(lock_t* l);

void release(lock_t* l);

void close_lock(lock_t* l);

#endif

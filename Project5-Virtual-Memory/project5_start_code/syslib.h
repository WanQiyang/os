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


#endif

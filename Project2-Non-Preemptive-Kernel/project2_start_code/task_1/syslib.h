/*  syslib.h
    Best viewed with tabs set to 4 spaces.
*/
#ifndef SYSLIB_H
#define SYSLIB_H


//  Prototypes for exported system calls
void yield(void);
void exit(void);
uint64_t get_timer(void);

#endif

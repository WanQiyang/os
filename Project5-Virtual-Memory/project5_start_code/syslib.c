/* syslib.c: Implementation of the system call library for the user processes. 
 * Each function implements a trap to the kernel. */
/* Do not change this file */

#include "common.h"
#include "syslib.h"
#include "util.h"

extern int invoke_syscall(int i, int arg1, int arg2, int arg3);

void yield(void){
     invoke_syscall(SYSCALL_YIELD, IGNORE, IGNORE, IGNORE);
}

void exit(void){
     invoke_syscall(SYSCALL_EXIT, IGNORE, IGNORE, IGNORE);
}

int getpid(void){
     return invoke_syscall(SYSCALL_GETPID, IGNORE, IGNORE, IGNORE);
}

void sleep(int milliseconds){
     invoke_syscall(SYSCALL_SLEEP, milliseconds, IGNORE, IGNORE);
}

void shutdown(void){
     invoke_syscall(SYSCALL_SHUTDOWN, IGNORE, IGNORE, IGNORE);
}

void write_serial(char c){
     invoke_syscall(SYSCALL_WRITE_SERIAL, (int)c, IGNORE, IGNORE);
}

/* Extra Credit */
int getpriority(void){
     return invoke_syscall(SYSCALL_GETPRIORITY, IGNORE, IGNORE, IGNORE);
}

/* Extra Credit */
void setpriority(int p){
     invoke_syscall(SYSCALL_SETPRIORITY, p, IGNORE, IGNORE);
}

int print_char(int line, int col, char c){
     invoke_syscall(SYSCALL_PRINT_CHAR, line, col, c);
}


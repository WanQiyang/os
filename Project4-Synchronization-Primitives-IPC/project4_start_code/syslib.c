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


pid_t spawn(const char *filename)
{
  return (pid_t) invoke_syscall(SYSCALL_SPAWN, (int)filename, IGNORE, IGNORE);
}

int kill(pid_t pid)
{
  return invoke_syscall(SYSCALL_KILL, (int)pid, IGNORE, IGNORE);
}

int wait(pid_t pid)
{
  return invoke_syscall(SYSCALL_WAIT, (int)pid, IGNORE, IGNORE);
}


mbox_t mbox_open(const char *name)
{
  return (mbox_t) invoke_syscall(SYSCALL_MBOX_OPEN, (int)name, IGNORE, IGNORE);
}

void mbox_close(mbox_t mbox)
{
  invoke_syscall(SYSCALL_MBOX_CLOSE, (int)mbox, IGNORE, IGNORE);
}

void mbox_send(mbox_t mbox, void *msg, int nbytes)
{
  invoke_syscall(SYSCALL_MBOX_SEND, (int)mbox, (int)msg, nbytes);
}

void mbox_recv(mbox_t mbox, void *msg, int nbytes)
{
  invoke_syscall(SYSCALL_MBOX_RECV, (int) mbox, (int)msg, nbytes);
}

void timer(void)
{
  return invoke_syscall(SYSCALL_TIMER, IGNORE, IGNORE, IGNORE);
}

void open_lock(lock_t* l) {
    invoke_syscall(SYSCALL_OPEN_LOCK, (int) l, IGNORE, IGNORE);
}

int acquire(lock_t* l) {
    return invoke_syscall(SYSCALL_ACQUIRE, (int) l, IGNORE, IGNORE);
}

void release(lock_t* l) {
    invoke_syscall(SYSCALL_RELEASE, (int) l, IGNORE, IGNORE);
}

void close_lock(lock_t* l) {
    invoke_syscall(SYSCALL_CLOSE_LOCK, (int) l, IGNORE, IGNORE);
}

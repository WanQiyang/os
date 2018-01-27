/* common.h */
/* Do not change this file */

#ifndef COMMON_H
#define COMMON_H

#define NULL ((void *) 0)

/* Extra Credit */
#define EC_PRIORITIES
#define EC_DEADLOCK

/* System calls
 * This enumeration gives every system call
 * a unique syscall number.
 * They are exposed to processes in syslib.h, eg yield()
 * They are exposed to kernel/threads in scheduler.h, eg do_yield()
 */
typedef enum {
    SYSCALL_YIELD=0,
    SYSCALL_EXIT,
    SYSCALL_GETPID,
    SYSCALL_GETPRIORITY,
    SYSCALL_SETPRIORITY,
    SYSCALL_SLEEP,
    SYSCALL_SHUTDOWN,
    SYSCALL_WRITE_SERIAL,
    SYSCALL_PRINT_CHAR,
    SYSCALL_SPAWN,
    SYSCALL_KILL,
    SYSCALL_WAIT,
    SYSCALL_MBOX_OPEN,
    SYSCALL_MBOX_CLOSE,
    SYSCALL_MBOX_SEND,
    SYSCALL_MBOX_RECV,
	SYSCALL_TIMER,
    SYSCALL_OPEN_LOCK,
    SYSCALL_ACQUIRE,
    SYSCALL_RELEASE,
    SYSCALL_CLOSE_LOCK,
    NUM_SYSCALLS
} syscall_t;

/* Assertion macros */
#define ASSERT2(p, s) \
do { \
    if (!(p)) { \
         print_str(0, 0, "Assertion failure: ");  \
         print_str(0, 19, s);                     \
         print_str(1, 0, "file: ");               \
         print_str(1, 6, __FILE__);               \
         print_str(2, 0, "line: ");               \
         print_int(2, 6, __LINE__);               \
        while (1); \
    } \
} while (0)
#define ASSERT(p) ASSERT2(p, #p)
#define HALT(s) ASSERT2(FALSE, s)

/* Commonly used types */
typedef enum {
    FALSE, TRUE
} bool_t;

typedef signed char int8_t;
typedef short int int16_t;
typedef long int32_t;
typedef long long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;

typedef int pid_t;
typedef int mbox_t;

/* A static upper limit on the
 * number of message boxes
 * at any time.
 */
#define MAX_MBOXEN          (32)

/* The longest name of a message
 * box.
 */
#define MBOX_NAME_LENGTH    (32)

/* A static upper limit on the
 * length of any one mbox.
 */
#define MAX_MBOX_LENGTH     (32)

/* The maximum length of any
 * message, it bytes
 */
#define MAX_MESSAGE_LENGTH  (32)

#include "queue.h"

typedef struct lock {
    node_t node;
    enum {
        UNLOCKED,
        LOCKED
    } status;
    node_t wait_queue;
    void* held_task;
} lock_t;

#endif                          /* COMMON_H */

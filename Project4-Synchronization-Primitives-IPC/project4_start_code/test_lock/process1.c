#include "common.h"
#include "syslib.h"
#include "util.h"
#include "printf.h"

void process1(void) {
    pid_t myPid = getpid();
    printf(3,1, "process1(%d): Created.", myPid);
    mbox_t sub = mbox_open("Publish-Lock");
    lock_t* ptr;
    mbox_recv(sub, &ptr, sizeof(lock_t*));
    printf(4,1, "process1: Acquiring lock... Address: %u.", (unsigned int) ptr);
    acquire(ptr);
    printf(5,1, "process1: Lock acquired. Wait to be killed...");
    while(1);
}

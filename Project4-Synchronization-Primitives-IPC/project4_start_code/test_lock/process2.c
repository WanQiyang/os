#include "common.h"
#include "syslib.h"
#include "util.h"
#include "printf.h"

void process2(void) {
    pid_t myPid = getpid();
    printf(7,1, "process2(%d): Created.", myPid);
    mbox_t sub = mbox_open("Publish-Lock");
    lock_t* ptr;
    mbox_recv(sub, &ptr, sizeof(lock_t*));
    printf(8,1, "process2: Acquiring lock... Address: %u.", (unsigned int) ptr);
    acquire(ptr);
    printf(10,1, "process2: Lock acquired. Success!");
    release(ptr);
    exit();
}

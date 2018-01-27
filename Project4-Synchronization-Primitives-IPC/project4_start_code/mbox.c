#include "common.h"
#include "mbox.h"
#include "sync.h"
#include "scheduler.h"


typedef struct
{
	/* TODO */
    char value[MAX_MESSAGE_LENGTH];
} Message;

typedef struct
{
	/* TODO */
    char name[MBOX_NAME_LENGTH];
    int count;
    int ptr;
    semaphore_t send, recv;
    Message msg[MAX_MBOX_LENGTH];
} MessageBox;


static MessageBox MessageBoxen[MAX_MBOXEN];
lock_t BoxLock;

/* Perform any system-startup
 * initialization for the message
 * boxes.
 */
void init_mbox(void)
{
	/* TODO */
    enter_critical();
    int i;
    for(i=0;i<MAX_MBOXEN;i++) {
        MessageBoxen[i].name[0] = 0;
        MessageBoxen[i].count = 0;
    }
    leave_critical();
}

/* Opens the mailbox named 'name', or
 * creates a new message box if it
 * doesn't already exist.
 * A message box is a bounded buffer
 * which holds up to MAX_MBOX_LENGTH items.
 * If it fails because the message
 * box table is full, it will return -1.
 * Otherwise, it returns a message box
 * id.
 */
mbox_t do_mbox_open(const char *name)
{
    /* (void)name; */
	/* TODO */
    enter_critical();
    int i;
    for(i=0;i<MAX_MBOXEN;i++) {
        if (same_string(name, MessageBoxen[i].name) && MessageBoxen[i].count > 0) {
            MessageBoxen[i].count++;
            leave_critical();
            return (mbox_t)i;
        }
    }
    for(i=0;i<MAX_MBOXEN;i++) {
        if (MessageBoxen[i].count == 0) {
            bcopy(name, MessageBoxen[i].name, strlen(name)+1);
            MessageBoxen[i].count++;
            MessageBoxen[i].ptr = 0;
            leave_critical();
            semaphore_init(&MessageBoxen[i].send, MAX_MBOX_LENGTH);
            semaphore_init(&MessageBoxen[i].recv, 0);
            return (mbox_t)i;
        }
    }
    ASSERT(FALSE);
}

/* Closes a message box
 */
void do_mbox_close(mbox_t mbox)
{
    /* (void)mbox; */
	/* TODO */
    enter_critical();
    MessageBoxen[mbox].count--;
    leave_critical();
}

/* Determine if the given
 * message box is full.
 * Equivalently, determine
 * if sending to this mbox
 * would cause a process
 * to block.
 */
int do_mbox_is_full(mbox_t mbox)
{
    /* (void)mbox; */
	/* TODO */
    enter_critical();
    return (MessageBoxen[mbox].send.value == 0);
    leave_critical();
}

/* Enqueues a message onto
 * a message box.  If the
 * message box is full, the
 * process will block until
 * it can add the item.
 * You may assume that the
 * message box ID has been
 * properly opened before this
 * call.
 * The message is 'nbytes' bytes
 * starting at offset 'msg'
 */
void do_mbox_send(mbox_t mbox, void *msg, int nbytes)
{
    /* (void)mbox;
    (void)msg;
    (void)nbytes; */
    /* TODO */
    semaphore_down(&MessageBoxen[mbox].send);
    enter_critical();
    MessageBoxen[mbox].ptr = (MessageBoxen[mbox].ptr + 1) % MAX_MBOX_LENGTH;
    bcopy(msg, &MessageBoxen[mbox].msg[MessageBoxen[mbox].ptr], nbytes);
    leave_critical();
    semaphore_up(&MessageBoxen[mbox].recv);
}

/* Receives a message from the
 * specified message box.  If
 * empty, the process will block
 * until it can remove an item.
 * You may assume that the
 * message box has been properly
 * opened before this call.
 * The message is copied into
 * 'msg'.  No more than
 * 'nbytes' bytes will by copied
 * into this buffer; longer
 * messages will be truncated.
 */
void do_mbox_recv(mbox_t mbox, void *msg, int nbytes)
{
    /*(void)mbox;
    (void)msg;
    (void)nbytes; */
    /* TODO */
    semaphore_down(&MessageBoxen[mbox].recv);
    enter_critical();
    bcopy(&MessageBoxen[mbox].msg[MessageBoxen[mbox].ptr], msg, nbytes);
    MessageBoxen[mbox].ptr = (MessageBoxen[mbox].ptr + MAX_MBOX_LENGTH - 1) % MAX_MBOX_LENGTH;
    leave_critical();
    semaphore_up(&MessageBoxen[mbox].send);
}

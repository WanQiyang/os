/* queue.h: queue implemented with a circular, doubly-linked list with a
   sentinel */
/* Do not change this file */

#ifndef QUEUE_H
#define QUEUE_H

typedef struct node {
    struct node *prev, *next;
} node_t;

/* Initialize a queue */
void queue_init(node_t * queue);

/* Remove and return the item at the front of the queue Return NULL if the
   queue is empty */
node_t *dequeue(node_t * queue);

/* Add item to the back of the queue */
void enqueue(node_t * queue, node_t * item);

/* Determine if the queue is empty.
 * Returns 1 if the queue is empty.
 * Returns 0 otherwise.
 */
int is_empty(node_t *queue);

/* Returns the first item in the queue
 * Returns NULL if the queue is empty
 */
node_t *peek(node_t *queue);

/* A comparison function should return:
 *  1 if a is less-than-or-equal-to b;
 *  0 otherwise.
 */
typedef int (*node_lte)(node_t *a, node_t *b);

/* Insert this item /elt/ into the queue /q/
 * in ascending order with the less-than-or-equal-to
 * inequality /comp/.
 * If /q/ was sorted (w.r.t. /comp/) before the
 * call, then /q/ is sorted after the call.
 * This is the simple linear-time algorithm.
 */
void enqueue_sort(node_t *q, node_t *item, node_lte comp);

#endif                          /* QUEUE_H */

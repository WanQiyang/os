/* queue.c */

#include "queue.h"
#include "common.h"

int test_line = 9;
bool_t queue_push(queue_t queue, pcb_t *pcb) {
	int i;
	// If the queue is full, return false
	if (!queue->isEmpty && (queue->head == queue->tail)) {
		return FALSE;
	}
	queue->pcbs[queue->tail] = pcb;
	queue->tail++;
	queue->tail %= queue->capacity;
	queue->isEmpty = FALSE; // manage isEmpty

	return TRUE;
}

/* Pop a pcb for a task off of the queue. */
pcb_t* queue_pop(queue_t queue) {
	// If the queue is empty, return null
	if (queue->isEmpty) {
		return NULL;
	}

	pcb_t *oPopped = queue->pcbs[queue->head];

	// set new head
	queue->head++;
	queue->head %= queue->capacity;
	// track whether queue was just made empty
	if (queue->head == queue->tail) {
		queue->isEmpty = TRUE;
	}

	return oPopped;
}

/* Initialize the queue */
void queue_init(queue_t queue) {
	queue->head = 0;
	queue->tail = 0;
	queue->isEmpty = TRUE;
}

/* Return the size of the queue */
uint32_t queue_size(queue_t queue) {
	uint32_t head = queue->head;
	uint32_t tail = queue->tail;
	return (queue->isEmpty) ? 0 : 
						(head < tail) ? (tail - head) : (tail + queue->capacity - head);
}


/* queue.c */
/* Do not change this file */

#include "common.h"
#include "queue.h"

void queue_init(node_t * queue){
  queue->prev = queue->next = queue;
}

node_t *dequeue(node_t * queue){
  node_t *item;
  
  item = queue->next;
  if (item == queue) {
    /* The queue is empty */
    item = NULL;
  } 
  else {
    /* Remove item from the queue */
    item->prev->next = item->next;
    item->next->prev = item->prev;
  }
  return item;
}

void enqueue(node_t * queue, node_t * item){
    item->prev = queue->prev;
    item->next = queue;
    item->prev->next = item;
    item->next->prev = item;
}

int is_empty(node_t *queue){
  if( queue->next == queue )
    return 1;
  else
    return 0;
}

node_t *peek(node_t *queue){
  if( queue->next == queue )
    return NULL;
  else
    return queue->next;
}

void enqueue_sort(node_t *q, node_t *elt, node_lte lte){
  node_t *iter;
  for(iter = q->next; iter && iter != q; iter=iter->next){
    if( lte(elt, iter) ) {
      /* put elt before iter */
      enqueue(iter, elt);
      return;
    }
  }
  
  /* Either the queue is empty, or
   * /elt/ is larger than all other elements.
   * put it at the end of the queue.
   */
  enqueue(q, elt);
}


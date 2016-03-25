#include "queue.h"
#include "types.h"
#include <string.h>

/*
 * Pushes a PCB pointer onto the queue. The pointer points to a PCB on
 * the process table.
 */
void queue_push( queue_t *q, pcb_t *pcb ) {
  int index = q->front + q->count;
  if ( index >= QUEUE_SZ ) {
    index -= QUEUE_SZ;
  }
  q->contents[ index ] = pcb;
  // memcpy( &q->contents[index], pcb, sizeof( pcb_t ) );
  q->count++;
}

/*
 * Does the opposite of the above.
 */
pcb_t *queue_pop( queue_t *q ) {
  if (!q->count) {
    return NULL;
  }
  pcb_t *result = q->contents[ q->front ];
  // memcpy( pcb, &q->contents[q->front], sizeof( pcb_t ) );
  q->front++;
  q->count--;
  if ( q->front >= QUEUE_SZ ) {
    q->front = 0;
  }
  return result;
}

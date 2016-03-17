#include "queue.h"
#include "types.h"
#include <string.h>

void push( queue_t *q, pcb_t *pcb ) {
  int index = q->front + q->count;
  if ( index >= QUEUE_SZ ) {
    index -= QUEUE_SZ;
  }
  memcpy( &q->contents[index], pcb, sizeof( pcb_t ) );
  q->count++;
}

int pop( queue_t *q, pcb_t *pcb ) {
  if (!q->count) {
    return 1;
  }
  memcpy( pcb, &q->contents[q->front], sizeof( pcb_t ) );
  q->front++;
  q->count--;
  if ( q->front >= QUEUE_SZ ) {
    q->front = 0;
  }
  return 0;
}

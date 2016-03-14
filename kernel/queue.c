#include "queue.h"
#include "types.h"
#include <string.h>

void push( queue_t *q, pcb_t *pcb ) {
  memcpy( &q->contents[q->front + q->count], pcb, sizeof( pcb_t ) );
  q->count++;
}

int pop( queue_t *q, pcb_t *pcb ) {
  if (!q->count) {
    return 1;
  }
  memcpy( pcb, &q->contents[q->front], sizeof( pcb_t ) );
  q->front++;
  q->count--;
  if ( q->front >= MAX_QUEUE_SZ ) {
    q->front = 0;
  }
  return 0;
}

#include "queue.h"
#include "types.h"
#include <string.h>

void push( queue_t *q, pcb_t *pcb ) {
  memcpy( &q->contents[q->front + q->count], pcb, sizeof( pcb_t ) );
  q->count++;

}

void pop( queue_t *q, pcb_t *pcb ) {
  memcpy( pcb, &q->contents[q->front], sizeof( pcb_t ) );
  q->front++;
  q->count--;
  if ( q->front >= MAX_QUEUE_SZ ) {
    q->front = 0;
  }
}

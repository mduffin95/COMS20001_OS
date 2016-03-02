#include "queue.h"

void push( queue_t *q, pcb_t *pcb ) {
  memcpy( q->contents[q->front + q->count], pcb, sizeof( pcb_t ) );
  q->count++;

}

pcb_t* pop( queue_t *q, pcb_t *ctx ) {
  pcb_t *result = &q->contents[q->front];
  q->front = (q->front + 1);
  if ( q->front >= MAX_QUEUE_SZ ) {
    q->front = 0;
  }
}

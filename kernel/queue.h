#ifndef __QUEUE_H
#define __QUEUE_H

#include "types.h"

#define QUEUE_SZ 10

typedef struct queue {
  pcb_t *contents[ QUEUE_SZ ];
  unsigned int front, count;
} queue_t;

void queue_push( queue_t *q, pcb_t *pcb );
pcb_t *queue_pop( queue_t *q );

#endif

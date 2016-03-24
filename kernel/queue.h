#ifndef __QUEUE_H
#define __QUEUE_H

#include "kernel.h"

#define QUEUE_SZ 10

typedef struct queue_t {
  pcb_t *contents[ QUEUE_SZ ];
  int front, count;
} queue_t;

void push( queue_t *q, pcb_t *pcb );
pcb_t *pop( queue_t *q );

#endif

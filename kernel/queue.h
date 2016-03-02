#ifndef __QUEUE_H
#define __QUEUE_H

#include "kernel.h"

#define MAX_QUEUE_SZ 10

typedef struct queue_t {
  pcb_t contents[ MAX_QUEUE_SZ ];
  int front, count;
  pcb_t *current;
} queue_t;

void push( queue_t *q, pcb_t *ctx );
void pop( queue_t *q, pcb_t *ctx );

#endif

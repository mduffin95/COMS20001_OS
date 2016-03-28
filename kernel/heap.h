#ifndef __HEAP_H
#define __HEAP_H

#include "types.h"

#define HEAP_SZ 32

typedef struct heap {
  pcb_t *contents[ HEAP_SZ ];
  unsigned int count;
} heap_t;

void heap_push( heap_t *h, pcb_t *pcb );
pcb_t *heap_peek( heap_t *h );
pcb_t *heap_pop( heap_t *h );

unsigned int parent( unsigned int i );
unsigned int left( unsigned int i );
unsigned int right( unsigned int i );

#endif

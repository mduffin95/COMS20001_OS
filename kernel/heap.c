#include "heap.h"

void heap_push( heap_t *h, pcb_t *pcb ) {
  unsigned int i = h->count++;
  while( i > 0 && h->contents[ parent( i ) ]->prty < pcb->prty ) {
    h->contents[ i ] = h->contents[ parent( i ) ];
    i = parent( i );
  }
  h->contents[ i ] = pcb;
}

pcb_t *heap_pop( heap_t *h ) {

}

unsigned int parent( unsigned int i ) {
  return ( i-1 ) >> 1;
}
unsigned int left( unsigned int i ) {
  return ( i<<1 ) + 1;
}
unsigned int right( unsigned int i ) {
  return ( i<<1 ) + 2;
}

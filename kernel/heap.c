#include "heap.h"
#include <string.h>

void heap_push( heap_t *h, pcb_t *pcb ) {
  unsigned int i = h->count++;
  while( i > 0 && h->contents[ parent( i ) ]->prty < pcb->prty ) {
    h->contents[ i ] = h->contents[ parent( i ) ];
    i = parent( i );
  }
  h->contents[ i ] = pcb;
}

pcb_t *heap_peek( heap_t *h ) {
  if ( !h->count ) {
    return NULL;
  }
  return h->contents[ 0 ];
}

pcb_t *heap_pop( heap_t *h ) {
  if ( !h->count ) {
    return NULL;
  }
  pcb_t *tmp, *result;
  result = h->contents[ 0 ];
  h->contents[ 0 ] = h->contents[ h->count-1 ];
  h->count--;
  unsigned int l, r, i, g;
  i = 0;
  while ( i < h->count ) {
    l = left( i );
    r = right( i );
    if( l < h->count && h->contents[ l ]->prty > h->contents[ i ]->prty) {
      g = l;
    }
    else {
      g = i;
    }
    if ( r < h->count && h->contents[ r ]->prty > h->contents[ g ]->prty) {
      g = r;
    }
    if ( g != i ) { //swap i and g
      tmp = h->contents[ g ];
      h->contents[ g ] = h->contents[ i ];
      h->contents[ i ] = tmp;
      i = g;
    }
    else break;
  }
  return result;
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

#include "types.h"
#include "io.h"

buffer_t in_buf = {0};
// buffer_t out_buf = {0};


void insert( buffer_t *buf, uint8_t x ) {
  int index = buf->front + buf->count;
  if ( index >= BUFFER_SZ ) {
    index -= BUFFER_SZ;
  }
  buf->contents[index] = x;
  buf->count++;
}

uint8_t remove( buffer_t *buf ) {
  if (!buf->count) {
    return 0;
  }
  uint8_t res = buf->contents[buf->front];
  buf->front++;
  buf->count--;
  if ( buf->front >= BUFFER_SZ ) {
    buf->front = 0;
  }
  return res;
}

// Returns the number of characters extracted from the buffer.
int extract_buf( uint8_t *x, size_t n ) {
  uint8_t y;
  int count = 0;
  while ( count < n && ( y = remove( &in_buf ) ) ) {
    *x = y;
    x++;
    count++;
  }
  return count;
}

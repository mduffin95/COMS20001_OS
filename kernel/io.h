#ifndef __IO_H
#define __IO_H

#include <stddef.h>

#define BUFFER_SZ 32

typedef struct buffer_t {
  uint8_t contents[ BUFFER_SZ ];
  int front, count;
} buffer_t;

void insert( buffer_t *buf, uint8_t x );
uint8_t remove( buffer_t *buf );
int extract_buf( uint8_t *x, size_t n );

extern buffer_t in_buf;

#endif

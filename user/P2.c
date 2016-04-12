#include "P2.h"
#include "libc.h"
#include "storage.h"
#include <string.h>

void P2() {
  format();

  uint8_t x[32];
  uint8_t y[32] = "hello world";

  int fd = open( "test.txt");

  while( 1 ) {
    if( write(fd, y, 1) ) {
      lseek( fd, -1, 0 );
    }
    if( read(fd, x, 1) ) {
      lseek( fd, -1, 0 );
    }
    if( !strcmp(x, y) ) {
      write(-1, "Test 1\n", 8 );
    }
  }
}

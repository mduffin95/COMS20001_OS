#include "P1.h"
#include "disk.h"
#include "storage.h"

void P1() {
  format();
  creat( "test" );
  int fd = open( "test" );

  uint8_t x[32];
  uint8_t y[32] =  "hello world";
  int z;
  while( 1 ) {
    if( z = write(fd, y, 12) ) {
      lseek( fd, -z, 0 );
    }
    if( z = read(fd, x, 12) ) {
      lseek( fd, -z, 0 );
    }
  }

}

void (*entry_P1)() = &P1;

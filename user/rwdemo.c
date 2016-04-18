#include "P1.h"
#include "disk.h"
#include "storage.h"

void rw_demo1() {
  format();
  creat( "foo" );
  creat( "bar" );
  int fd1 = open( "foo" );
  int fd2 = open( "bar" );

  uint8_t x[32] = "foo text";
  uint8_t y[32] = "bar text";
  int z;
  if( z = write(fd1, x, 9) ) {
    lseek( fd1, -z, SEEK_CUR );
  }
  if( z = write(fd2, y, 9) ) {
    lseek( fd2, -z, SEEK_CUR );
  }
  close( fd1 );
  close( fd2 );
  unlink( "foo" );
  unlink( "bar" );

  while( 1 ) {
  }

}

void (*entry_P1)() = &P1;

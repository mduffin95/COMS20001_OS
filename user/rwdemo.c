#include "P1.h"
#include "disk.h"
#include "storage.h"

void rw_demo() {
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

  uint8_t buffer[32];
  if( z = read(fd1, buffer, 9) ) {
    lseek( fd1, -z, SEEK_CUR );
  }
  write(-1, buffer, 9);
  if( z = read(fd2, buffer, 9) ) {
    lseek( fd2, -z, SEEK_CUR );
  }
  write(-1, buffer, 9);

  close( fd1 );
  close( fd2 );
  unlink( "foo" );
  unlink( "bar" );

  if( open( "foo" ) == -1 ) {
    write( -1, "\nfoo cannot be opened\n", 23 );
  }

  while(1);
}

void (*entry_P1)() = &P1;

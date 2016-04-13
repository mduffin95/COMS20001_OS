#include "P1.h"
#include "disk.h"
#include "storage.h"

void P1() {
  // char* x = "hello world, I'm P1\n";

  // while( 1 ) {
  //   uint8_t c = chan_receive(0);
  //   write( 0, &c, 1 ); //yield();
  // }

  format();

  int fd = open( "test.txt");

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

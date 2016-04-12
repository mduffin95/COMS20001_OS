#include "P1.h"
#include "disk.h"

void P1() {
  // char* x = "hello world, I'm P1\n";

  // while( 1 ) {
  //   uint8_t c = chan_receive(0);
  //   write( 0, &c, 1 ); //yield();
  // }

  uint8_t x[64];
  while( 1 ) {
    disk_rd( 258*32+1, x, 63 );
    write( -1, x, 64 );
  }

}

void (*entry_P1)() = &P1;

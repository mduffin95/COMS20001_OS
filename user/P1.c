#include "P1.h"

void P1() {
  // char* x = "hello world, I'm P1\n";

  while( 1 ) {
    uint8_t c = chan_receive(0);
    write( 0, &c, 1 ); //yield();
  }
}

void (*entry_P1)() = &P1;

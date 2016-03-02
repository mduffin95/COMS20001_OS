#include "P1.h"

void P1() {
  char* x = "hello world, I'm P1\n";

  while( 1 ) {
    write( 0, x, 20 ); //yield();
  }
}

void (*entry_P1)() = &P1;

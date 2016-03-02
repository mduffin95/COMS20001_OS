#include "P0.h"
// #include <stdlib.h>

void P0() {
  fork();
  // char* x = "hello world, I'm P0\n";
  char x = 0x30;
  char newline = '\n';

  //Write an int to string function and test.

  while( 1 ) {
    write( 0, &x, 1 ); //yield();
    write( 0, &newline, 1);
    x++;
    if(x > 0x39) {
      x = 0x30;
    }
  }
}

void (*entry_P0)() = &P0;

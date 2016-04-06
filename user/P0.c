#include "P0.h"
#include "types.h"
// #include <stdlib.h>

void P0() {
  // char type;
  char x = 0x30;
  // char newline = '\n';
  // pid_t pid = fork();
  // if (pid) {
  //   type = 'P';
  // }
  // else {
  //   type = 'C';
  // }


  //Write an int to string function and test.

  while( 1 ) {
    // write( 0, &type, 1 );
    // write( 0, &x, 1 ); //yield();
    // write( 0, &newline, 1);
    x++;
    // if( type == 'P' && count-- == 0 ) {
    //   exit();
    // }
    chan_send(0, x);
    if(x > 0x39) {
      x = 0x30;
    }
  }
}

void (*entry_P0)() = &P0;

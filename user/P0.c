#include "P0.h"
#include "types.h"
// #include <stdlib.h>

void P0() {
  pid_t pid = fork();
  char type;
  char x = 0x30;
  char newline = '\n';
  int count = 10000;
  if (pid) {
    type = 'P';
  }
  else {
    type = 'C';
  }
  // char* x = "hello world, I'm P0\n";


  //Write an int to string function and test.

  while( 1 ) {
    write( 0, &type, 1 );
    write( 0, &x, 1 ); //yield();
    write( 0, &newline, 1);
    x++;
    if( type == 'P' && count-- == 0 ) {
      exit();
    }
    if(x > 0x39) {
      x = 0x30;
    }
  }
}

void (*entry_P0)() = &P0;

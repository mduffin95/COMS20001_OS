#include "philosophers.h"
#include <stdint.h>
#include "libc.h"

void phil( const char *eat, uint8_t left, uint8_t right ) {
  int eating = 0;
  while(1) {
    chan_receive(left); //Pick up left fork
    chan_receive(right); //Pick up right fork
    for(int i = 0; i<EAT; i++) {
      write( -1, (void*) eat, 10 );
    }
    chan_send(right, 1); //Put right fork down
    chan_send(left, 1); //Put left fork down
  }
}

void din_phil( void ) {
  for(int i=0; i<5; i++) {
    chan_set_ready(i);
  }
  pid_t pid = fork();
  if( !pid ) {
    phil( "Eating 0\n", 0, 4 );
  }
  pid = fork();
  if( !pid ) {
    phil( "Eating 1\n", 1, 0 );
  }
  pid = fork();
  if( !pid ) {
    phil( "Eating 2\n", 2, 1 );
  }
  pid = fork();
  if( !pid ) {
    phil( "Eating 3", 3, 2 );
  }
  phil( "Eating 4", 4, 3 );
}

void ipc_send( void ) {
   char x = 0x30;

   while( 1 ) {
     x++;
     chan_send(0, x);
    if(x > 0x39) {
      x = 0x30;
    }
   }
}

void ipc_rcv( void ) {
  while( 1 ) {
    uint8_t c = chan_receive(0);
    write( -1, &c, 1 );
  }
}

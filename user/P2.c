#include "P2.h"
#include "libc.h"
#include "storage.h"
// int is_prime( uint32_t x ) {
//   if ( !( x & 1 ) || ( x < 2 ) ) {
//     return ( x == 2 );
//   }
//
//   for( uint32_t d = 3; ( d * d ) <= x ; d += 2 ) {
//     if( !( x % d ) ) {
//       return 0;
//     }
//   }
//
//   return 1;
// }
//
// void P2() {
//   int x = 0;
//
//   while( 1 ) {
//     // test whether each x for 2^8 < x < 2^24 is prime or not
//
//     for( uint32_t x = ( 1 << 8 ); x < ( 1 << 24 ); x++ ) {
//       int r = is_prime( x ); // printf( "is_prime( %d ) = %d\n", x, r );
//     }
//   }
//
//   return;
// }

void P2() {
  format();

  uint8_t x[32];
  uint8_t y[32] = {'h','e','l','l','o',' ','w','o','r','l','d','\0'};

  int fd = open( "test.txt");

  while( 1 ) {
    if( read(fd, x, 1) ) {
      lseek( fd, -1, 0 );
    }
    if( write(fd, y, 1) ) {
      lseek( fd, -1, 0 );
    }
  }
}
